#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <limits>
#include <iomanip>
#include <memory>
#include "linear.h"
#include "em.h"
#include "common.h"
#include "params.h"
#include "mat.h"
#include "serialize.h"
#include "lda.h"
#include "drawer.h"

using namespace std;
using namespace Eigen;

// this is a huge hack because I'm too lazy to pass the drawer pointer all the way down here.
static drawer draw("/tmp/viewer");

void draw_mode_prediction(const std::string &obj, int mode) {
	static double mode_colors[][3] = {
		{ 0.0, 0.0, 0.0 },
		{ 1.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 1.0 },
		{ 1.0, 1.0, 0.0 },
		{ 0.0, 1.0, 1.0 },
		{ 1.0, 0.0, 1.0 }
	};
	static int ncolors = sizeof(mode_colors) / sizeof(mode_colors[0]);
	double *colors;
	
	if (mode >= ncolors)
		mode = ncolors - 1;
	
	colors = mode_colors[mode];
	draw.set_color(obj, colors[0], colors[1], colors[2]);
}

bool approx_equal(double a, double b) {
	return fabs(a - b) / min(fabs(a), fabs(b)) < .001;
}


void read_til_semi(istream &is, vector<double> &buf) {
	string s;
	while (true) {
		double x;
		is >> s;
		if (s == ";") {
			return;
		}
		if (!parse_double(s, x)) {
			assert(false);
		}
		buf.push_back(x);
	}
}

/* Box-Muller method */
double randgauss(double mean, double std) {
	double x1, x2, w;
	do {
		x1 = 2.0 * ((double) rand()) / RAND_MAX - 1.0;
		x2 = 2.0 * ((double) rand()) / RAND_MAX - 1.0;
		w = x1 * x1 + x2 * x2;
	} while (w >= 1.0);
	
	w = sqrt((-2.0 * log(w)) / w);
	return mean + std * (x1 * w);
}

void kernel1(const cvec &d, cvec &w) {
	w.resize(d.size());
	for (int i = 0; i < d.size(); ++i) {
		w(i) = exp(-d(i));
	}
}

void kernel2(const cvec &d, cvec &w, double p) {
	const double maxw = 1.0e9;
	w.resize(d.size());
	for (int i = 0; i < d.size(); ++i) {
		if (d(i) == 0.0) {
			w(i) = maxw;
		} else {
			w(i) = min(maxw, pow(d(i), p));
		}
	}
}

void predict(const mat &C, const rvec &intercepts, const rvec &x, rvec &y) {
	y = (x * C) + intercepts;
}

/*
 Upon return, X and Y will contain the training data, Xtest and Ytest the
 testing data.
*/
void split_data(
	const_mat_view X,
	const_mat_view Y,
	const vector<int> &use,
	int ntest,
	mat &Xtrain, mat &Xtest, 
	mat &Ytrain, mat &Ytest)
{
	int ntrain = use.size() - ntest;
	vector<int> test;
	sample(ntest, 0, use.size(), test);
	sort(test.begin(), test.end());
	
	int train_end = 0, test_end = 0, i = 0;
	for (int j = 0; j < use.size(); ++j) {
		if (i < test.size() && j == test[i]) {
			Xtest.row(test_end) = X.row(use[j]);
			Ytest.row(test_end) = Y.row(use[j]);
			++test_end;
			++i;
		} else {
			Xtrain.row(train_end) = X.row(use[j]);
			Ytrain.row(train_end) = Y.row(use[j]);
			++train_end;
		}
	}
	assert(test_end == ntest && train_end == ntrain);
}

/*
 Add tuples from a single time point into the relation table
*/
void extend_relations(relation_table &rels, const relation_table &add, int time) {
	relation_table::const_iterator i, iend;
	relation::const_iterator j, jend;
	tuple t;
	
	for (i = add.begin(), iend = add.end(); i != iend; ++i) {
		const string &name = i->first;
		const relation &r = i->second;
		relation &r2 = rels[name];
		if (r2.arity() == 0) {
			r2.reset(r.arity());
		}
		for (j = r.begin(), jend = r.end(); j != jend; ++j) {
			t = *j;
			t[0] = time;
			r2.add(t);
		}
	}
}

void push_back_unique(tuple &t, int e) {
	if (find(t.begin(), t.end(), e) == t.end()) {
		t.push_back(e);
	}
}

void get_context_rels(int target, const relation_table &rels, relation_table &context_rels) {
	tuple close;
	relation::const_iterator i, iend;
	
	/*
	 If the target intersects any objects, those are all considered close. If the
	 target doesn't intersect any objects, then the closest object is considered
	 close.
	*/
	const relation &intersect_rel = map_get(rels, string("intersect"));
	for (i = intersect_rel.begin(), iend = intersect_rel.end(); i != iend; ++i) {
		if ((*i)[1] == target) {
			push_back_unique(close, (*i)[2]);
		} else if ((*i)[2] == target) {
			push_back_unique(close, (*i)[1]);
		}
	}
	if (close.empty()) {
		const relation &closest_rel = map_get(rels, string("closest"));
		for (i = closest_rel.begin(), iend = closest_rel.end(); i != iend; ++i) {
			if ((*i)[1] == target) {
				close.push_back((*i)[2]);
				break;
			}
		}
	}
	close.push_back(target);
	
	// filter out all far objects
	context_rels = rels;
	relation_table::iterator j, jend;
	for (j = context_rels.begin(), jend = context_rels.end(); j != jend; ++j) {
		relation &r = j->second;
		if (j->first == "closest") {
			r.clear();
		} else {
			for (int j = 1, jend = r.arity(); j < jend; ++j) {
				r.filter(j, close, false);
			}
		}
	}
}


/*
 Assume that data from a single mode comes in blocks. Try to discover
 a mode by randomly fitting a line to a block of data and then finding
 all data that fit the line.
*/
void EM::find_linear_subset_block(const_mat_view X, const_mat_view Y, vector<int> &subset) const {
	function_timer t(timers.get_or_add("block_subset"));
	
	int xcols = X.cols();
	int rank = xcols + 1;
	int ndata = X.rows();
	mat Xb(rank, xcols), Yb(xcols+1, 1), coefs;
	
	int start = rand() % (ndata - rank);
	for (int i = 0; i < rank; ++i) {
		Xb.row(i) = X.row(start + i);
		Yb.row(i) = Y.row(start + i);
	}
	linreg_clean(FORWARD, Xb, Yb, coefs);

	cvec errors = (Y - (X * coefs)).col(0).array().abs();
	subset.clear();
	for (int i = 0; i < ndata; ++i) {
		if (errors(i) < MODEL_ERROR_THRESH) {
			subset.push_back(i);
		}
	}
}


/*
 Use a simple version of EM to discover a mode in noise data. This
 method works better than block_seed when data from a single mode
 doesn't come in contiguous blocks.
 
 The algorithm is:
 
 1. If input X has m non-static columns, assume it has rank = m + 1.
 2. Randomly choose 'rank' training points as the seed members for the linear
    function. Fit the function to the seed members.
 3. Compute the residuals of the function for training data. Compute a weight
    vector based on the residuals and a kernel.
 4. Refit the linear function biased based on the weight vector. Repeat until
    convergence or the function fits at least n data points.
*/
void EM::find_linear_subset_em(const_mat_view X, const_mat_view Y, vector<int> &subset) const {
	function_timer t(timers.get_or_add("em_block"));
	
	int ndata = X.rows(), xcols = X.cols();
	vector<int> init, nonuniform_cols;
	cvec w(ndata), error(ndata), old_error(ndata);
	mat Xc(ndata, xcols), Yc(ndata, 1), coefs(xcols, 1), coefs2;
	
	error.setConstant(INFINITY);
	sample(xcols + 1, 0, ndata, init);
	w.setConstant(0.0);
	for (int i = 0; i < init.size(); ++i) {
		w(init[i]) = 1.0;
	}

	for (int iter = 0; iter < MINI_EM_MAX_ITERS; ++iter) {
		int i = 0;
		nonuniform_cols.clear();
		for (int j = 0; j < xcols; ++j) {
			Xc.col(i) = X.col(j).array() * w.array();
			if (!uniform(Xc.col(i))) {
				nonuniform_cols.push_back(j);
				++i;
			}
		}
		Yc.col(0) = Y.col(0).array() * w.array();
		if (i == 0 || uniform(Yc.col(0)))
			return;
			
		if (!linreg_clean(FORWARD, Xc.leftCols(i), Yc, coefs2)) {
			assert(false);
		}
		coefs.setConstant(0);
		for (int j = 0; j < nonuniform_cols.size(); ++j) {
			coefs.row(nonuniform_cols[j]) = coefs2.row(j);
		}
		
		old_error = error;
		error = (Y - (X * coefs)).col(0).array().abs();
		if (error.maxCoeff() <= MODEL_ERROR_THRESH)
			break;
		
		double diff = (error - old_error).array().abs().maxCoeff();
		if (iter > 0 && diff < SAME_THRESH) {
			break;
		}
		kernel2(error, w, -3.0);
	}
	for (int i = 0; i < ndata; ++i) {
		if (error(i) < MODEL_ERROR_THRESH) {
			subset.push_back(i);
		}
	}
}

void erase_inds(vector<int> &v, const vector<int> &inds) {
	int i = 0, j = 0;
	for (int k = 0; k < v.size(); ++k) {
		if (i < inds.size() && k == inds[i]) {
			++i;
		} else {
			if (j < k) {
				v[j] = v[k];
			}
			++j;
		}
	}
	assert(i == inds.size() && j == v.size() - inds.size());
	v.resize(j);
}

int EM::find_linear_subset(mat &X, mat &Y, vector<int> &subset, mat &coefs, rvec &inter) const {
	function_timer t(timers.get_or_add("find_seed"));

	const double TEST_RATIO = 0.5;
	
	int orig_xcols = X.cols();
	int largest = 0, nleft = X.rows();
	
	// preprocess the data as much as possible
	vector<int> used_cols;
	clean_lr_data(X, used_cols);
	augment_ones(X);

	mat Xsub(X.rows(), X.cols()), Ysub(Y.rows(), 1);
	rvec avg_error;

	vector<int> ungrouped(nleft);
	for (int i = 0; i < nleft; ++i) {
		ungrouped[i] = i;
	}
	
	/*
	 Outer loop ranges over sets of random initial points
	*/
	for (int iter = 0; iter < LINEAR_SUBSET_MAX_ITERS; ++iter) {
		vector<int> subset2;
		find_linear_subset_em(X.topRows(nleft), Y.topRows(nleft), subset2);
		if (subset2.size() < 10)  // arbitrary, fix later
			continue;
		
		pick_rows(X, subset2, Xsub);
		pick_rows(Y, subset2, Ysub);
		nfoldcv(Xsub.topRows(subset2.size()), Ysub.topRows(subset2.size()), 5, FORWARD, avg_error);
		if (avg_error(0) > MODEL_ERROR_THRESH) {
			/*
			 There isn't a clear linear relationship between the points, so I can't
			 consider them a single block.
			*/
			continue;
		}
		
		if (subset2.size() > largest) {
			subset.clear();
			for (int i = 0; i < subset2.size(); ++i) {
				subset.push_back(ungrouped[subset2[i]]);
			}
			largest = subset.size();
			if (largest >= NEW_MODE_THRESH) {
				mat subcoefs;
				linreg(FORWARD, Xsub.topRows(subset2.size()), Ysub.topRows(subset2.size()), cvec(), subcoefs, inter);
				coefs.resize(orig_xcols, Y.cols());
				coefs.setConstant(0.0);
				for (int i = 0; i < used_cols.size(); ++i) {
					coefs.row(used_cols[i]) = subcoefs.row(i);
				}
				return largest;
			}
		}
		
		/*
		 Assume this group of points won't fit linearly in any other group, so they
		 can be excluded from consideration in the next iteration.
		*/
		pick_rows(X, subset2);
		erase_inds(ungrouped, subset2);
		nleft = ungrouped.size();
		if (nleft < NEW_MODE_THRESH) {
			break;
		}
	}
	return largest;
}

template <typename T>
void remove_from_vector(const vector<int> &inds, vector <T> &v) {
	int i = 0, j = 0;
	for (int k = 0; k < v.size(); ++k) {
		if (i < inds.size() && k == inds[i]) {
			++i;
		} else {
			if (k > j) {
				v[j] = v[k];
			}
			j++;
		}
	}
	assert(v.size() - inds.size() == j);
	v.resize(j);
}

EM::EM() 
: ndata(0), nmodes(1), use_em(true), use_foil(true), use_foil_close(true),
  use_pruning(true), use_unify(true), learn_new_modes(true),
  check_after(NEW_MODE_THRESH), nc_type(NC_DTREE)
{
	mode_info *noise = new mode_info(true, false, data, sigs);
	noise->classifiers.resize(1, NULL);
	modes.push_back(noise);
}

EM::~EM() {
	clear_and_dealloc(data);
	clear_and_dealloc(modes);
}


void EM::learn(int target, const scene_sig &sig, const relation_table &rels, const rvec &x, const rvec &y) {
	function_timer t(timers.get_or_add("learn"));
	
	int sig_index = -1;
	for (int i = 0; i < sigs.size(); ++i) {
		if (sigs[i]->sig == sig) {
			sig_index = i;
			break;
		}
	}
	
	if (sig_index < 0) {
		sig_info *si = new sig_info;
		si->sig = sig;
		sigs.push_back(si);
		sig_index = sigs.size() - 1;
	}

	em_train_data *d = new em_train_data;
	d->x = x;
	d->y = y;
	d->target = target;
	d->sig_index = sig_index;
	sigs[sig_index]->members.push_back(ndata);
	
	/*
	 Remember that because the LWR object is initialized with alloc = false, it's
	 just going to store pointers to these rvecs rather than duplicate them.
	*/
	sigs[sig_index]->lwr.learn(d->x, d->y);
	
	d->mode = 0;
	d->minfo.resize(nmodes);
	d->minfo[0].prob = PNOISE;
	d->minfo[0].prob_stale = false;
	data.push_back(d);
	
	modes[0]->add_example(ndata);
	noise_by_sig[d->sig_index].insert(ndata);
	extend_relations(rel_tbl, rels, ndata);
	
	relation_table context_rels;
	get_context_rels(sig[target].id, rels, context_rels);
	extend_relations(context_rel_tbl, context_rels, ndata);
	++ndata;
}

void EM::estep() {
	function_timer t(timers.get_or_add("e-step"));
	
	/*
	 For data i and mode j, if:
	 
	  * P(i, j) increases and j was not the MAP mode, or
	  * P(i, j) decreases and j was the MAP mode
	 
	 then we mark i as a point we have to recalculate the MAP mode for.
	*/
	for (int i = 0; i < ndata; ++i) {
		em_train_data &d = *data[i];
		bool stale = false;
		for (int j = 1; j < nmodes; ++j) {
			em_train_data::data_mode_info &dm = d.minfo[j];
			if (!dm.prob_stale && !modes[j]->is_new_fit()) {
				continue;
			}
			double prev = d.minfo[d.mode].prob, now, error;
			now = modes[j]->calc_prob(d.target, sigs[d.sig_index]->sig, d.x, d.y(0), dm.obj_map, error);
			assert(dm.obj_map.size() == modes[j]->get_sig().size());
			if ((d.mode == j && now < prev) || (d.mode != j && now > dm.prob)) {
				stale = true;
			}
			dm.prob = now;
			dm.prob_stale = false;
		}
		if (stale) {
			int prev = d.mode, best = 0;
			for (int j = 1; j < nmodes; ++j) {
				/*
				 These conditions look awkward, but have justification. If I tested the >
				 condition before the approx_equal condition, the test would succeed even
				 if the probability of j was only slightly better than the probability of
				 best.
				*/
				if (approx_equal(d.minfo[j].prob, d.minfo[best].prob)) {
					if (modes[j]->get_num_nonzero_coefs() < modes[best]->get_num_nonzero_coefs()) {
						best = j;
					}
				} else if (d.minfo[j].prob > d.minfo[best].prob) {
					best = j;
				}
			}
			if (best != prev) {
				d.mode = best;
				modes[prev]->del_example(i);
				if (prev == 0) {
					noise_by_sig[d.sig_index].erase(i);
				}
				modes[best]->add_example(i);
				if (best == 0) {
					noise_by_sig[d.sig_index].insert(i);
				}
			}
		}
	}
	
	for (int i = 1; i < modes.size(); ++i) {
		modes[i]->reset_new_fit();
	}
}

bool EM::mstep() {
	function_timer t(timers.get_or_add("m-step"));
	
	bool changed = false;
	for (int i = 1; i < nmodes; ++i) {
		changed = changed || modes[i]->update_fits();
	}
	return changed;
}

void EM::fill_xy(const vector<int> &rows, mat &X, mat &Y) const {
	if (rows.empty()) {
		X.resize(0, 0);
		Y.resize(0, 0);
		return;
	}

	X.resize(rows.size(), data[rows[0]]->x.size());
	Y.resize(rows.size(), 1);

	for (int i = 0; i < rows.size(); ++i) {
		X.row(i) = data[rows[i]]->x;
		Y.row(i) = data[rows[i]]->y;
	}
}

mode_info *EM::add_mode(bool manual) {
	mode_info *new_mode = new mode_info(false, manual, data, sigs);
	modes.push_back(new_mode);
	++nmodes;
	for (int j = 0; j < ndata; ++j) {
		grow(data[j]->minfo);
	}
	for (int j = 0; j < nmodes; ++j) {
		modes[j]->classifiers.resize(nmodes, NULL);
		/*
		 It's sufficient to fill the extra vector elements
		 with NULL here. The actual classifiers will be
		 allocated as needed during updates.
		*/
	}
	return new_mode;
}

bool EM::unify_or_add_mode() {
	function_timer t(timers.get_or_add("new"));

	assert(check_after >= NEW_MODE_THRESH);
	if (!learn_new_modes || modes[0]->size() < check_after) {
		return false;
	}
	
	vector<int> largest;
	mat coefs;
	rvec inter;
	modes[0]->largest_const_subset(largest);
	int potential = largest.size();
	inter = data[largest[0]]->y; // constant model
	if (largest.size() < NEW_MODE_THRESH) {
		mat X, Y;
		map<int, set<int> >::const_iterator i;
		for (i = noise_by_sig.begin(); i != noise_by_sig.end(); ++i) {
			if (i->second.size() < check_after) {
				if (i->second.size() > potential) {
					potential = i->second.size();
				}
				continue;
			}
			vector<int> indvec(i->second.begin(), i->second.end()), subset;
			fill_xy(indvec, X, Y);
			find_linear_subset(X, Y, subset, coefs, inter);
			if (subset.size() > potential) {
				potential = subset.size();
			}
			if (subset.size() > largest.size()) {
				largest.clear();
				for (int i = 0; i < subset.size(); ++i) {
					largest.push_back(indvec[subset[i]]);
				}
				if (largest.size() >= NEW_MODE_THRESH) {
					break;
				}
			}
		}
	}
	
	if (largest.size() < NEW_MODE_THRESH) {
		check_after += (NEW_MODE_THRESH - potential);
		return false;
	}
	
	/*
	 From here I know the noise data is going to either become a new mode or unify
	 with an existing mode, so reset check_after assuming the current noise is
	 gone.
	*/
	check_after = NEW_MODE_THRESH;
	
	int seed_sig = data[largest[0]]->sig_index;
	int seed_target = data[largest[0]]->target;

	if (use_unify) {
		/*
		 Try to add noise data to each current model and refit. If the resulting
		 model is just as accurate as the original, then just add the noise to that
		 model instead of creating a new one.
		*/
		mat X, Y, ucoefs;
		rvec uinter;
		for (int j = 1; j < nmodes; ++j) {
			mode_info &minfo = *modes[j];
	
			if (minfo.is_manual() || !minfo.uniform_sig(seed_sig, seed_target)) {
				continue;
			}
	
			vector<int> combined, subset;
			extend(combined, minfo.get_members());
			extend(combined, largest);
			fill_xy(combined, X, Y);
			LOG(EMDBG) << "Trying to unify with mode " << j << endl;
			int unified_size = find_linear_subset(X, Y, subset, ucoefs, uinter);
			
			if (unified_size >= minfo.size() + .9 * largest.size()) {
				LOG(EMDBG) << "Successfully unified with mode " << j << endl;
				const em_train_data &d0 = *data[combined[subset[0]]];
				minfo.set_linear_params(d0.sig_index, d0.target, ucoefs, uinter);
				return true;
			}
			LOG(EMDBG) << "Failed to unify with mode " << j << endl;
		}
	}
	
	mode_info *new_mode = add_mode(false);
	const em_train_data &d0 = *data[largest[0]];
	new_mode->set_linear_params(d0.sig_index, d0.target, coefs, inter);
	return true;
}


bool EM::predict(int target, const scene_sig &sig, const relation_table &rels, const rvec &x, int &mode, rvec &y) {
	if (ndata == 0) {
		mode = 0;
		return false;
	}
	
	vector<int> obj_map;
	if (use_em) {
		mode = classify(target, sig, rels, x, obj_map);
		draw_mode_prediction(sig[target].name, mode);
		if (mode > 0) {
			modes[mode]->predict(sig, x, obj_map, y);
			return true;
		}
	}
	for (int i = 0; i < sigs.size(); ++i) {
		if (sigs[i]->sig == sig) {
			if (sigs[i]->lwr.predict(x, y)) {
				return true;
			}
			break;
		}
	}
	y(0) = NAN;
	return false;
}

/*
 Remove all modes that cover fewer than NEW_MODE_THRESH data points.
*/
bool EM::remove_modes() {
	if (nmodes == 1) {
		return false;
	}
	
	/*
	 i is the first free model index. If model j should be kept, all
	 information pertaining to model j will be copied to row/element i
	 in the respective matrix/vector, and i will be incremented. Most
	 efficient way I can think of to remove elements from the middle
	 of vectors. index_map associates old j's to new i's.
	*/
	vector<int> index_map(nmodes), removed;
	int i = 1;  // start with 1, noise mode (0) should never be removed
	for (int j = 1; j < nmodes; ++j) {
		if (modes[j]->size() >= NEW_MODE_THRESH || modes[j]->is_manual()) {
			index_map[j] = i;
			if (j > i) {
				modes[i] = modes[j];
			}
			i++;
		} else {
			index_map[j] = 0;
			delete modes[j];
			removed.push_back(j);
		}
	}
	if (removed.empty()) {
		return false;
	}
	assert(i == nmodes - removed.size());
	nmodes = i;
	modes.resize(nmodes);
	for (int j = 0; j < nmodes; ++j) {
		remove_from_vector(removed, modes[j]->classifiers);
	}
	for (int j = 0; j < ndata; ++j) {
		em_train_data &d = *data[j];
		if (d.mode >= 0) {
			d.mode = index_map[d.mode];
		}
		remove_from_vector(removed, d.minfo);
	}
	return true;
}

bool EM::run(int maxiters) {
	if (use_em) {
		for (int i = 0; i < maxiters; ++i) {
			estep();
			bool changed = mstep();
			if (!changed && !remove_modes() && !unify_or_add_mode()) {
				// reached quiescence
				return true;
			}
		}
		LOG(EMDBG) << "Reached max iterations without quiescence" << endl;
	}
	return false;
}

int EM::best_mode(int target, const scene_sig &sig, const rvec &x, double y, double &besterror) const {
	int best = -1;
	double best_prob = 0.0;
	rvec py;
	vector<int> assign;
	double error;
	for (int i = 0; i < nmodes; ++i) {
		double p = modes[i]->calc_prob(target, sig, x, y, assign, error);
		if (best == -1 || p > best_prob) {
			best = i;
			best_prob = p;
			besterror = error;
		}
	}
	return best;
}

bool EM::cli_inspect(int first, const vector<string> &args, ostream &os) {
	if (first >= args.size()) {
		os << "modes: " << nmodes << endl;
		os << endl << "subqueries: mode ptable timing train relations classifiers use_em use_foil use_nc" << endl;
		return true;
	} else if (args[first] == "ptable") {
		table_printer t;
		for (int i = 0; i < ndata; ++i) {
			t.add_row() << i;
			for (int j = 0; j < nmodes; ++j) {
				t << data[i]->minfo[j].prob;
			}
		}
		t.print(os);
		return true;
	} else if (args[first] == "train") {
		return cli_inspect_train(first + 1, args, os);
	} else if (args[first] == "dump_train") {
		return cli_dump_train(first + 1, args, os);
	} else if (args[first] == "mode") {
		if (first + 1 >= args.size()) {
			os << "Specify a mode number (0 - " << nmodes - 1 << ")" << endl;
			return false;
		}
		int n;
		if (!parse_int(args[first+1], n) || n < 0 || n >= nmodes) {
			os << "invalid mode number" << endl;
			return false;
		}
		return modes[n]->cli_inspect(first + 2, args, os);
	} else if (args[first] == "timing") {
		timers.report(os);
		return true;
	} else if (args[first] == "relations") {
		return cli_inspect_relations(first + 1, args, os);
	} else if (args[first] == "classifiers") {
		return cli_inspect_classifiers(first + 1, args, os);
	} else if (args[first] == "use_em") {
		return read_on_off(args, first + 1, os, use_em);
	} else if (args[first] == "use_foil") {
		return read_on_off(args, first + 1, os, use_foil);
	} else if (args[first] == "use_foil_close") {
		return read_on_off(args, first + 1, os, use_foil_close);
	} else if (args[first] == "use_nc") {
		return read_on_off(args, first + 1, os, use_nc);
	} else if (args[first] == "use_pruning") {
		return read_on_off(args, first + 1, os, use_pruning);
	} else if (args[first] == "use_unify") {
		return read_on_off(args, first + 1, os, use_unify);
	} else if (args[first] == "learn_new_modes") {
		return read_on_off(args, first + 1, os, learn_new_modes);
	} else if (args[first] == "dump_foil" || args[first] == "dump_foil_close") {
		int m1, m2;
		if (first + 2 >= args.size() || 
		    !parse_int(args[first+1], m1) || 
		    !parse_int(args[first+2], m2) ||
		    m1 < 0 || m1 >= nmodes || m2 < 0 || m2 >= nmodes || m1 == m2) 
		{
			os << "Specify 2 modes" << endl;
			return false;
		}
		
		if (m1 > m2)
			swap(m1, m2);
		
		FOIL foil;
		if (args[first] == "dump_foil") {
			foil.set_problem(modes[m1]->get_member_rel(), modes[m2]->get_member_rel(), rel_tbl);
		} else {
			foil.set_problem(modes[m1]->get_member_rel(), modes[m2]->get_member_rel(), context_rel_tbl);
		}
		foil.dump_foil6(os);
		return true;
	} else if (args[first] == "vistrain") {
		int i;
		
		if (first + 1 >= args.size() || !parse_int(args[first+1], i) || i < 0 || i >= data.size()) {
			os << "specify training example" << endl;
			return false;
		}
		
		rvec &x = data[i]->x;
		const scene_sig &s = sigs[data[i]->sig_index]->sig;
		
		for (int j = 0, jend = s.size(); j < jend; ++j) {
			vec3 p;
			vector<bool> found(3, false);
			int start = s[j].start;
			const vector<string> &props = s[j].props;
			for (int k = 0, kend = props.size(); k < kend; ++k) {
				if (props[k] == "px") {
					p(0) = x(start + k);
					found[0] = true;
				} else if (props[k] == "py") {
					p(1) = x(start + k);
					found[1] = true;
				} else if (props[k] == "pz") {
					p(2) = x(start + k);
					found[2] = true;
				}
			}
			if (found[0] && found[1] && found[2]) {
				draw.set_pos(s[j].name, p(0), p(1), p(2));
			}
		}
		return true;
	} else if (args[first] == "add_mode") {
		return cli_add_mode(first + 1, args, os);
	} else if (args[first] == "nc_type") {
		if (first + 1 >= args.size()) {
			os << get_num_classifier_name(nc_type) << endl;
			return true;
		}
		int t = get_num_classifier_type(args[first + 1]);
		if (t < 0) {
			os << "no such numeric classifier";
			return false;
		}
		nc_type = t;
		os << "future numeric classifiers will be learned using " << args[first+1] << endl;
		return true;
	}

	return false;
}

bool EM::cli_inspect_train(int first, const vector<string> &args, ostream &os) const {
	int start = 0, end = ndata - 1;
	vector<string> objs;
	bool have_start = false;
	for (int i = first; i < args.size(); ++i) {
		int x;
		if (parse_int(args[i], x)) {
			if (!have_start) {
				start = x;
				have_start = true;
			} else {
				end = x;
			}
		} else {
			objs.push_back(args[i]);
		}		
	}

	if (start < 0 || end < start || end >= ndata) {
		os << "invalid data range" << endl;
		return false;
	}

	vector<int> cols;
	table_printer t;
	t.set_scientific(true);
	t.set_precision(10);
	t.add_row() << "N" << "CLS" << "|" << "DATA";
	for (int i = start; i <= end; ++i) {
		if (i == start || (i > start && data[i]->sig_index != data[i-1]->sig_index)) {
			const scene_sig &s = sigs[data[i]->sig_index]->sig;
			t.add_row().skip(2) << "|";
			int c = 0;
			cols.clear();
			for (int j = 0; j < s.size(); ++j) {
				if (objs.empty() || has(objs, s[j].name)) {
					for (int k = 0; k < s[j].props.size(); ++k) {
						cols.push_back(c++);
					}
					t << s[j].name;
					t.skip(s[j].props.size() - 1);
				} else {
					c += s[j].props.size();
				}
			}
			t.add_row().skip(2) << "|";
			for (int j = 0; j < s.size(); ++j) {
				if (objs.empty() || has(objs, s[j].name)) {
					const vector<string> &props = s[j].props;
					for (int k = 0; k < props.size(); ++k) {
						t << props[k];
					}
				}
			}
		}
		t.add_row();
		t << i << data[i]->mode << "|";
		for (int j = 0; j < cols.size(); ++j) {
			t << data[i]->x(cols[j]);
		}
		t << data[i]->y(0);
	}
	t.print(os);
	return true;
}

bool EM::cli_dump_train(int first, const vector<string> &args, ostream &os) const {
	if (first >= args.size()) {
		os << "specify path" << endl;
		return false;
	}
	ofstream dump(args[first].c_str());
	if (!dump) {
		os << "invalid path" << endl;
		return false;
	}
		
	int xcols = data[0]->x.cols();
	int ycols = data[0]->y.cols();
	mat train(data.size(), xcols + ycols);
	for (int i = 0, iend = data.size(); i < iend; ++i) {
		train.block(i, 0, 1, xcols) = data[i]->x;
		train.block(i, xcols, 1, ycols) = data[i]->y;
	}
	::serialize(train, dump);
	return true;
}

/*
 The format will be [coef] [dim] [coef] [dim] ... [intercept]
*/
bool EM::cli_add_mode(int first, const vector<string> &args, ostream &os) {
	if (ndata == 0) {
		os << "need at least one training example to get the signature from" << endl;
		return false;
	}
	
	int target = data[ndata-1]->target;
	int sig_index = data[ndata-1]->sig_index;
	const scene_sig &sig = sigs[sig_index]->sig;
	mat coefs(sig.dim(), 1);
	rvec intercept(1);
	coefs.setConstant(0.0);
	intercept.setConstant(0.0);
	
	for (int i = first, iend = args.size(); i < iend; i += 2) {
		double c;
		if (!parse_double(args[i], c)) {
			os << "expecting a number, got " << args[i] << endl;
			return false;
		}
		
		if (i + 1 >= args.size()) {
			intercept(0) = c;
			break;
		}
		
		vector<string> parts;
		split(args[i+1], ":", parts);
		if (parts.size() != 2) {
			os << "expecting object:property, got " << args[i+1] << endl;
			return false;
		}
		
		int obj_ind, prop_ind;
		if (!sig.get_dim(parts[0], parts[1], obj_ind, prop_ind)) {
			os << args[i+1] << " not found" << endl;
			return false;
		}
		assert(prop_ind >= 0 && prop_ind < coefs.rows());
		coefs(prop_ind, 0) = c;
	}
	
	mode_info *new_mode = add_mode(true);
	new_mode->set_linear_params(sig_index, target, coefs, intercept);
	return true;
}


void EM::serialize(ostream &os) const {
	serializer(os) << ndata << nmodes << data << sigs << rel_tbl << context_rel_tbl << noise_by_sig << nc_type;
	for (int i = 0; i < nmodes; ++i) {
		modes[i]->serialize(os);
	}
}

void EM::unserialize(istream &is) {
	unserializer(is) >> ndata >> nmodes >> data >> sigs >> rel_tbl >> context_rel_tbl >> noise_by_sig >> nc_type;
	assert(data.size() == ndata);
	
	delete modes[0];
	modes.clear();
	for (int i = 0; i < nmodes; ++i) {
		mode_info *m = new mode_info(i == 0, false, data, sigs);
		m->unserialize(is);
		modes.push_back(m);
	}
	
	for (int i = 0; i < sigs.size(); ++i) {
		sig_info &si = *sigs[i];
		for (int j = 0; j < si.members.size(); ++j) {
			const em_train_data &d = *data[si.members[j]];
			si.lwr.learn(d.x, d.y);
		}
	}
}

void em_train_data::serialize(ostream &os) const {
	serializer(os) << target << sig_index << x << y << mode << minfo;
}

void em_train_data::unserialize(istream &is) {
	unserializer(is) >> target >> sig_index >> x >> y >> mode >> minfo;
}


bool EM::cli_inspect_relations(int i, const vector<string> &args, ostream &os) const {
	const relation_table *rels;
	if (i < args.size() && args[i] == "close") {
		rels = &context_rel_tbl;
		++i;
	} else {
		rels = &rel_tbl;
	}
	
	if (i >= args.size()) {
		os << *rels << endl;
		return true;
	}
	const relation *r = map_getp(*rels, args[i]);
	if (!r) {
		os << "no such relation" << endl;
		return false;
	}
	if (i + 1 >= args.size()) {
		os << *r << endl;
		return true;
	}

	relation matches(*r);

	tuple t(1);
	int j, k;
	for (j = i + 1, k = 0; j < args.size() && k < matches.arity(); ++j, ++k) {
		if (args[j] != "*") {
			if (!parse_int(args[j], t[0])) {
				os << "invalid pattern" << endl;
				return false;
			}
			matches.filter(k, t, false);
		}
	}

	os << matches << endl;
	return true;
}

void EM::update_classifier() {
	
	vector<bool> needs_update(modes.size(), false);
	for (int i = 0; i < modes.size(); ++i) {
		if (modes[i]->classifier_stale) {
			needs_update[i] = true;
			modes[i]->classifier_stale = false;
		}
	}
	
	for (int i = 0; i < modes.size(); ++i) {
		mode_info &minfo = *modes[i];

		if (needs_update[i]) {
			minfo.learn_obj_clauses(rel_tbl);
		}
		
		for (int j = i + 1; j < modes.size(); ++j) {
			if (needs_update[i] || needs_update[j]) {
				update_pair(i, j);
			}
		}
	}
}

void EM::update_pair(int i, int j) {
	function_timer t(timers.get_or_add("updt_clsfr"));
	
	assert(i < j);
	if (!modes[i]->classifiers[j]) {
		modes[i]->classifiers[j] = new classifier(use_foil, use_pruning, nc_type);
	}
	classifier &c = *(modes[i]->classifiers[j]);
	const relation &mem_i = modes[i]->get_member_rel();
	const relation &mem_j = modes[j]->get_member_rel();
	if (use_foil_close) {
		c.update(mem_i, mem_j, context_rel_tbl, data, sigs);
	} else {
		c.update(mem_i, mem_j, rel_tbl, data, sigs);
	}
}

/*
 Return 0 to vote for i, 1 to vote for j
*/
int EM::vote_pair(int i, int j, int target, const scene_sig &sig, const relation_table &rels, const rvec &x) const {
	LOG(EMDBG) << "Voting on " << i << " vs " << j << endl;
	
	assert(modes[i]->classifiers[j]);
	const classifier &c = *(modes[i]->classifiers[j]);

	if (use_foil_close) {
		relation_table context;
		get_context_rels(sig[target].id, rels, context);
		return c.vote(target, sig, context, x);
	} else {
		return c.vote(target, sig, rels, x);
	}
}

int EM::classify(int target, const scene_sig &sig, const relation_table &rels, const rvec &x, vector<int> &obj_map) {
	LOG(EMDBG) << "classification" << endl;
	update_classifier();
	
	/*
	 The scene has to contain the objects used by the linear model of
	 a mode for it to possibly qualify for that mode.
	*/
	vector<int> possible(1, 0);
	map<int, vector<int> > mappings;
	for (int i = 1; i < modes.size(); ++i) {
		mode_info &minfo = *modes[i];
		if (minfo.get_sig().size() > sig.size()) {
			continue;
		}
		if (!minfo.map_objs(target, sig, rels, mappings[i])) {
			LOG(EMDBG) << "mapping failed for " << i << endl;
			continue;
		}
		possible.push_back(i);
	}
	if (possible.size() == 1) {
		LOG(EMDBG) << "only one possible mode: " << possible[0] << endl;
		obj_map = mappings[possible[0]];
		return possible[0];
	}
	
	map<int, int> votes;
	for (int i = 0; i < possible.size() - 1; ++i) {
		int a = possible[i];
		for (int j = i + 1; j < possible.size(); ++j) {
			int b = possible[j];
			int winner = vote_pair(a, b, target, sig, rels, x);
			if (winner == 0) {
				++votes[a];
			} else if (winner == 1) {
				++votes[b];
			} else {
				assert(false);
			}
		}
	}
	
	LOG(EMDBG) << "votes:" << endl;
	map<int, int>::const_iterator i, best = votes.begin();
	for (i = votes.begin(); i != votes.end(); ++i) {
		LOG(EMDBG) << i->first << " = " << i->second << endl;
		if (i->second > best->second) {
			best = i;
		}
	}
	LOG(EMDBG) << "best mode = " << best->first << endl;
	obj_map = mappings[best->first];
	return best->first;
}

bool EM::cli_inspect_classifiers(int first, const vector<string> &args, ostream &os) const {
	const_cast<EM*>(this)->update_classifier();
	
	if (first >= args.size()) {
		// print summary of all classifiers
		for (int i = 0; i < nmodes; ++i) {
			for (int j = 0; j < nmodes; ++j) {
				if (j <= i) {
					continue;
				}
				classifier *c = modes[i]->classifiers[j];
				os << "=== FOR MODES " << i << "/" << j << " ===" << endl;
				if (c) {
					c->inspect(os);
				} else {
					// why would this happen?
					os << "no classifier" << endl;
				}
			}
		}
		return true;
	}
	
	int i, j;
	if (first + 1 >= args.size()) {
		os << "Specify two modes" << endl;
		return false;
	}
	
	if (!parse_int(args[first], i) || !parse_int(args[first+1], j) || 
	    i >= j || i >= modes.size() || j >= modes.size())
	{
		os << "invalid modes, make sure i < j" << endl;
		return false;
	}
	
	classifier *c = modes[i]->classifiers[j];
	if (c) {
		c->inspect_detailed(os);
	} else {
		os << "no classifier" << endl;
	}
	return true;
}

sig_info::sig_info() : lwr(LWR_K, false) {}

void sig_info::serialize(ostream &os) const {
	serializer(os) << sig << members;
}

void sig_info::unserialize(istream &is) {
	unserializer(is) >> sig >> members;
}

void em_train_data::data_mode_info::serialize(ostream &os) const {
	serializer(os) << prob << prob_stale << obj_map;
}

void em_train_data::data_mode_info::unserialize(istream &is) {
	unserializer(is) >> prob >> prob_stale >> obj_map;
}
