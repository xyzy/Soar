/* LCM type definition class file
 * This file was automatically generated by lcm-gen
 * DO NOT MODIFY BY HAND!!!!
 */

package edu.umich.robot.lcmtypes;
 
import java.io.*;
import java.nio.*;
import java.util.*;
import lcm.lcm.*;
 
public final class tag_pose_t implements lcm.lcm.LCMEncodable
{
    public long utime;
    public int ntags;
    public int id[];
    public double poses[][][];
 
    public tag_pose_t()
    {
    }
 
    public static final long LCM_FINGERPRINT;
    public static final long LCM_FINGERPRINT_BASE = 0x40b4e7b9ae840f2fL;
 
    static {
        LCM_FINGERPRINT = _hashRecursive(new ArrayList<Class>());
    }
 
    public static long _hashRecursive(ArrayList<Class> classes)
    {
        if (classes.contains(tag_pose_t.class))
            return 0L;
 
        classes.add(tag_pose_t.class);
        long hash = LCM_FINGERPRINT_BASE
            ;
        classes.remove(classes.size() - 1);
        return (hash<<1) + ((hash>>63)&1);
    }
 
    public void encode(DataOutput outs) throws IOException
    {
        outs.writeLong(LCM_FINGERPRINT);
        _encodeRecursive(outs);
    }
 
    public void _encodeRecursive(DataOutput outs) throws IOException
    {
        outs.writeLong(this.utime); 
 
        outs.writeInt(this.ntags); 
 
        for (int a = 0; a < this.ntags; a++) {
            outs.writeInt(this.id[a]); 
        }
 
        for (int a = 0; a < this.ntags; a++) {
            for (int b = 0; b < 4; b++) {
                for (int c = 0; c < 4; c++) {
                    outs.writeDouble(this.poses[a][b][c]); 
                }
            }
        }
 
    }
 
    public tag_pose_t(byte[] data) throws IOException
    {
        this(new LCMDataInputStream(data));
    }
 
    public tag_pose_t(DataInput ins) throws IOException
    {
        if (ins.readLong() != LCM_FINGERPRINT)
            throw new IOException("LCM Decode error: bad fingerprint");
 
        _decodeRecursive(ins);
    }
 
    public static tag_pose_t _decodeRecursiveFactory(DataInput ins) throws IOException
    {
        tag_pose_t o = new tag_pose_t();
        o._decodeRecursive(ins);
        return o;
    }
 
    public void _decodeRecursive(DataInput ins) throws IOException
    {
        this.utime = ins.readLong();
 
        this.ntags = ins.readInt();
 
        this.id = new int[(int) ntags];
        for (int a = 0; a < this.ntags; a++) {
            this.id[a] = ins.readInt();
        }
 
        this.poses = new double[(int) ntags][(int) 4][(int) 4];
        for (int a = 0; a < this.ntags; a++) {
            for (int b = 0; b < 4; b++) {
                for (int c = 0; c < 4; c++) {
                    this.poses[a][b][c] = ins.readDouble();
                }
            }
        }
 
    }
 
    public tag_pose_t copy()
    {
        tag_pose_t outobj = new tag_pose_t();
        outobj.utime = this.utime;
 
        outobj.ntags = this.ntags;
 
        outobj.id = new int[(int) ntags];
        if (this.ntags > 0)
            System.arraycopy(this.id, 0, outobj.id, 0, this.ntags); 
        outobj.poses = new double[(int) ntags][(int) 4][(int) 4];
        for (int a = 0; a < this.ntags; a++) {
            for (int b = 0; b < 4; b++) {
                System.arraycopy(this.poses[a][b], 0, outobj.poses[a][b], 0, 4);            }
        }
 
        return outobj;
    }
 
}

