/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version: 1.3.22
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package sml;

public class sml_Names {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected sml_Names(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(sml_Names obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public void delete() {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      smlJNI.delete_sml_Names(swigCPtr);
    }
    swigCPtr = 0;
  }

  public static String getKTagSML() {
    return smlJNI.get_sml_Names_kTagSML();
  }

  public static String getKID() {
    return smlJNI.get_sml_Names_kID();
  }

  public static String getKAck() {
    return smlJNI.get_sml_Names_kAck();
  }

  public static String getKDocType() {
    return smlJNI.get_sml_Names_kDocType();
  }

  public static String getKDocType_Call() {
    return smlJNI.get_sml_Names_kDocType_Call();
  }

  public static String getKDocType_Response() {
    return smlJNI.get_sml_Names_kDocType_Response();
  }

  public static String getKDocType_Notify() {
    return smlJNI.get_sml_Names_kDocType_Notify();
  }

  public static String getKSoarVersion() {
    return smlJNI.get_sml_Names_kSoarVersion();
  }

  public static String getKSMLVersion() {
    return smlJNI.get_sml_Names_kSMLVersion();
  }

  public static String getKSMLVersionValue() {
    return smlJNI.get_sml_Names_kSMLVersionValue();
  }

  public static String getKSoarVersionValue() {
    return smlJNI.get_sml_Names_kSoarVersionValue();
  }

  public static String getKOutputLinkName() {
    return smlJNI.get_sml_Names_kOutputLinkName();
  }

  public static String getKTagCommand() {
    return smlJNI.get_sml_Names_kTagCommand();
  }

  public static String getKCommandName() {
    return smlJNI.get_sml_Names_kCommandName();
  }

  public static String getKCommandOutput() {
    return smlJNI.get_sml_Names_kCommandOutput();
  }

  public static String getKRawOutput() {
    return smlJNI.get_sml_Names_kRawOutput();
  }

  public static String getKStructuredOutput() {
    return smlJNI.get_sml_Names_kStructuredOutput();
  }

  public static String getKTagArg() {
    return smlJNI.get_sml_Names_kTagArg();
  }

  public static String getKArgParam() {
    return smlJNI.get_sml_Names_kArgParam();
  }

  public static String getKArgType() {
    return smlJNI.get_sml_Names_kArgType();
  }

  public static String getKTagError() {
    return smlJNI.get_sml_Names_kTagError();
  }

  public static String getKErrorCode() {
    return smlJNI.get_sml_Names_kErrorCode();
  }

  public static String getKTagName() {
    return smlJNI.get_sml_Names_kTagName();
  }

  public static String getKTagResult() {
    return smlJNI.get_sml_Names_kTagResult();
  }

  public static String getKValueDelta() {
    return smlJNI.get_sml_Names_kValueDelta();
  }

  public static String getKValueFull() {
    return smlJNI.get_sml_Names_kValueFull();
  }

  public static String getKTagWME() {
    return smlJNI.get_sml_Names_kTagWME();
  }

  public static String getKWME_TimeTag() {
    return smlJNI.get_sml_Names_kWME_TimeTag();
  }

  public static String getKWME_Id() {
    return smlJNI.get_sml_Names_kWME_Id();
  }

  public static String getKWME_Attribute() {
    return smlJNI.get_sml_Names_kWME_Attribute();
  }

  public static String getKWME_Value() {
    return smlJNI.get_sml_Names_kWME_Value();
  }

  public static String getKWME_ValueType() {
    return smlJNI.get_sml_Names_kWME_ValueType();
  }

  public static String getKWME_Action() {
    return smlJNI.get_sml_Names_kWME_Action();
  }

  public static String getKValueAdd() {
    return smlJNI.get_sml_Names_kValueAdd();
  }

  public static String getKValueRemove() {
    return smlJNI.get_sml_Names_kValueRemove();
  }

  public static String getKTypeString() {
    return smlJNI.get_sml_Names_kTypeString();
  }

  public static String getKTypeInt() {
    return smlJNI.get_sml_Names_kTypeInt();
  }

  public static String getKTypeDouble() {
    return smlJNI.get_sml_Names_kTypeDouble();
  }

  public static String getKTypeChar() {
    return smlJNI.get_sml_Names_kTypeChar();
  }

  public static String getKTypeBoolean() {
    return smlJNI.get_sml_Names_kTypeBoolean();
  }

  public static String getKTypeID() {
    return smlJNI.get_sml_Names_kTypeID();
  }

  public static String getKTypeVariable() {
    return smlJNI.get_sml_Names_kTypeVariable();
  }

  public static String getKParamAgent() {
    return smlJNI.get_sml_Names_kParamAgent();
  }

  public static String getKParamKernel() {
    return smlJNI.get_sml_Names_kParamKernel();
  }

  public static String getKParamThis() {
    return smlJNI.get_sml_Names_kParamThis();
  }

  public static String getKParamName() {
    return smlJNI.get_sml_Names_kParamName();
  }

  public static String getKParamFilename() {
    return smlJNI.get_sml_Names_kParamFilename();
  }

  public static String getKParamLearning() {
    return smlJNI.get_sml_Names_kParamLearning();
  }

  public static String getKParamOSupportMode() {
    return smlJNI.get_sml_Names_kParamOSupportMode();
  }

  public static String getKParamValue() {
    return smlJNI.get_sml_Names_kParamValue();
  }

  public static String getKParamWme() {
    return smlJNI.get_sml_Names_kParamWme();
  }

  public static String getKParamWmeObject() {
    return smlJNI.get_sml_Names_kParamWmeObject();
  }

  public static String getKParamAttribute() {
    return smlJNI.get_sml_Names_kParamAttribute();
  }

  public static String getKParamCount() {
    return smlJNI.get_sml_Names_kParamCount();
  }

  public static String getKParamLength() {
    return smlJNI.get_sml_Names_kParamLength();
  }

  public static String getKParamThread() {
    return smlJNI.get_sml_Names_kParamThread();
  }

  public static String getKParamProcess() {
    return smlJNI.get_sml_Names_kParamProcess();
  }

  public static String getKParamLine() {
    return smlJNI.get_sml_Names_kParamLine();
  }

  public static String getKParamLocation() {
    return smlJNI.get_sml_Names_kParamLocation();
  }

  public static String getKParamLogLocation() {
    return smlJNI.get_sml_Names_kParamLogLocation();
  }

  public static String getKParamLogLevel() {
    return smlJNI.get_sml_Names_kParamLogLevel();
  }

  public static String getKParamInputProducer() {
    return smlJNI.get_sml_Names_kParamInputProducer();
  }

  public static String getKParamOutputProcessor() {
    return smlJNI.get_sml_Names_kParamOutputProcessor();
  }

  public static String getKParamWorkingMemory() {
    return smlJNI.get_sml_Names_kParamWorkingMemory();
  }

  public static String getKParamAttributePath() {
    return smlJNI.get_sml_Names_kParamAttributePath();
  }

  public static String getKParamUpdate() {
    return smlJNI.get_sml_Names_kParamUpdate();
  }

  public static String getKParamEventID() {
    return smlJNI.get_sml_Names_kParamEventID();
  }

  public static String getKParamLearnSetting() {
    return smlJNI.get_sml_Names_kParamLearnSetting();
  }

  public static String getKParamLearnOnlySetting() {
    return smlJNI.get_sml_Names_kParamLearnOnlySetting();
  }

  public static String getKParamLearnExceptSetting() {
    return smlJNI.get_sml_Names_kParamLearnExceptSetting();
  }

  public static String getKParamLearnAllLevelsSetting() {
    return smlJNI.get_sml_Names_kParamLearnAllLevelsSetting();
  }

  public static String getKParamLearnForceLearnStates() {
    return smlJNI.get_sml_Names_kParamLearnForceLearnStates();
  }

  public static String getKParamLearnDontLearnStates() {
    return smlJNI.get_sml_Names_kParamLearnDontLearnStates();
  }

  public static String getKParamLogSetting() {
    return smlJNI.get_sml_Names_kParamLogSetting();
  }

  public static String getKParamDirectory() {
    return smlJNI.get_sml_Names_kParamDirectory();
  }

  public static String getKParamSeconds() {
    return smlJNI.get_sml_Names_kParamSeconds();
  }

  public static String getKParamWarningsSetting() {
    return smlJNI.get_sml_Names_kParamWarningsSetting();
  }

  public static String getKParamPhase() {
    return smlJNI.get_sml_Names_kParamPhase();
  }

  public static String getKParamInstance() {
    return smlJNI.get_sml_Names_kParamInstance();
  }

  public static String getKParamTimers() {
    return smlJNI.get_sml_Names_kParamTimers();
  }

  public static String getKParamMessage() {
    return smlJNI.get_sml_Names_kParamMessage();
  }

  public static String getKParamAlias() {
    return smlJNI.get_sml_Names_kParamAlias();
  }

  public static String getKParamAliasedCommand() {
    return smlJNI.get_sml_Names_kParamAliasedCommand();
  }

  public static String getKParamIndifferentSelectionMode() {
    return smlJNI.get_sml_Names_kParamIndifferentSelectionMode();
  }

  public static String getKParamNumericIndifferentMode() {
    return smlJNI.get_sml_Names_kParamNumericIndifferentMode();
  }

  public static String getKParamRunResult() {
    return smlJNI.get_sml_Names_kParamRunResult();
  }

  public static String getKParamVersionMajor() {
    return smlJNI.get_sml_Names_kParamVersionMajor();
  }

  public static String getKParamVersionMinor() {
    return smlJNI.get_sml_Names_kParamVersionMinor();
  }

  public static String getKParamVersionMicro() {
    return smlJNI.get_sml_Names_kParamVersionMicro();
  }

  public static String getKParamWaitSNC() {
    return smlJNI.get_sml_Names_kParamWaitSNC();
  }

  public static String getKParamFunction() {
    return smlJNI.get_sml_Names_kParamFunction();
  }

  public static String getKParamChunkNamePrefix() {
    return smlJNI.get_sml_Names_kParamChunkNamePrefix();
  }

  public static String getKParamChunkCount() {
    return smlJNI.get_sml_Names_kParamChunkCount();
  }

  public static String getKParamChunkLongFormat() {
    return smlJNI.get_sml_Names_kParamChunkLongFormat();
  }

  public static String getKParamStatsProductionCountDefault() {
    return smlJNI.get_sml_Names_kParamStatsProductionCountDefault();
  }

  public static String getKParamStatsProductionCountUser() {
    return smlJNI.get_sml_Names_kParamStatsProductionCountUser();
  }

  public static String getKParamStatsProductionCountChunk() {
    return smlJNI.get_sml_Names_kParamStatsProductionCountChunk();
  }

  public static String getKParamStatsProductionCountJustification() {
    return smlJNI.get_sml_Names_kParamStatsProductionCountJustification();
  }

  public static String getKParamStatsCycleCountDecision() {
    return smlJNI.get_sml_Names_kParamStatsCycleCountDecision();
  }

  public static String getKParamStatsCycleCountElaboration() {
    return smlJNI.get_sml_Names_kParamStatsCycleCountElaboration();
  }

  public static String getKParamStatsProductionFiringCount() {
    return smlJNI.get_sml_Names_kParamStatsProductionFiringCount();
  }

  public static String getKParamStatsWmeCountAddition() {
    return smlJNI.get_sml_Names_kParamStatsWmeCountAddition();
  }

  public static String getKParamStatsWmeCountRemoval() {
    return smlJNI.get_sml_Names_kParamStatsWmeCountRemoval();
  }

  public static String getKParamStatsWmeCount() {
    return smlJNI.get_sml_Names_kParamStatsWmeCount();
  }

  public static String getKParamStatsWmeCountAverage() {
    return smlJNI.get_sml_Names_kParamStatsWmeCountAverage();
  }

  public static String getKParamStatsWmeCountMax() {
    return smlJNI.get_sml_Names_kParamStatsWmeCountMax();
  }

  public static String getKParamStatsKernelTimeTotal() {
    return smlJNI.get_sml_Names_kParamStatsKernelTimeTotal();
  }

  public static String getKParamStatsMatchTimeInputPhase() {
    return smlJNI.get_sml_Names_kParamStatsMatchTimeInputPhase();
  }

  public static String getKParamStatsMatchTimeDetermineLevelPhase() {
    return smlJNI.get_sml_Names_kParamStatsMatchTimeDetermineLevelPhase();
  }

  public static String getKParamStatsMatchTimePreferencePhase() {
    return smlJNI.get_sml_Names_kParamStatsMatchTimePreferencePhase();
  }

  public static String getKParamStatsMatchTimeWorkingMemoryPhase() {
    return smlJNI.get_sml_Names_kParamStatsMatchTimeWorkingMemoryPhase();
  }

  public static String getKParamStatsMatchTimeOutputPhase() {
    return smlJNI.get_sml_Names_kParamStatsMatchTimeOutputPhase();
  }

  public static String getKParamStatsMatchTimeDecisionPhase() {
    return smlJNI.get_sml_Names_kParamStatsMatchTimeDecisionPhase();
  }

  public static String getKParamStatsOwnershipTimeInputPhase() {
    return smlJNI.get_sml_Names_kParamStatsOwnershipTimeInputPhase();
  }

  public static String getKParamStatsOwnershipTimeDetermineLevelPhase() {
    return smlJNI.get_sml_Names_kParamStatsOwnershipTimeDetermineLevelPhase();
  }

  public static String getKParamStatsOwnershipTimePreferencePhase() {
    return smlJNI.get_sml_Names_kParamStatsOwnershipTimePreferencePhase();
  }

  public static String getKParamStatsOwnershipTimeWorkingMemoryPhase() {
    return smlJNI.get_sml_Names_kParamStatsOwnershipTimeWorkingMemoryPhase();
  }

  public static String getKParamStatsOwnershipTimeOutputPhase() {
    return smlJNI.get_sml_Names_kParamStatsOwnershipTimeOutputPhase();
  }

  public static String getKParamStatsOwnershipTimeDecisionPhase() {
    return smlJNI.get_sml_Names_kParamStatsOwnershipTimeDecisionPhase();
  }

  public static String getKParamStatsChunkingTimeInputPhase() {
    return smlJNI.get_sml_Names_kParamStatsChunkingTimeInputPhase();
  }

  public static String getKParamStatsChunkingTimeDetermineLevelPhase() {
    return smlJNI.get_sml_Names_kParamStatsChunkingTimeDetermineLevelPhase();
  }

  public static String getKParamStatsChunkingTimePreferencePhase() {
    return smlJNI.get_sml_Names_kParamStatsChunkingTimePreferencePhase();
  }

  public static String getKParamStatsChunkingTimeWorkingMemoryPhase() {
    return smlJNI.get_sml_Names_kParamStatsChunkingTimeWorkingMemoryPhase();
  }

  public static String getKParamStatsChunkingTimeOutputPhase() {
    return smlJNI.get_sml_Names_kParamStatsChunkingTimeOutputPhase();
  }

  public static String getKParamStatsChunkingTimeDecisionPhase() {
    return smlJNI.get_sml_Names_kParamStatsChunkingTimeDecisionPhase();
  }

  public static String getKParamStatsMemoryUsageMiscellaneous() {
    return smlJNI.get_sml_Names_kParamStatsMemoryUsageMiscellaneous();
  }

  public static String getKParamStatsMemoryUsageHash() {
    return smlJNI.get_sml_Names_kParamStatsMemoryUsageHash();
  }

  public static String getKParamStatsMemoryUsageString() {
    return smlJNI.get_sml_Names_kParamStatsMemoryUsageString();
  }

  public static String getKParamStatsMemoryUsagePool() {
    return smlJNI.get_sml_Names_kParamStatsMemoryUsagePool();
  }

  public static String getKParamStatsMemoryUsageStatsOverhead() {
    return smlJNI.get_sml_Names_kParamStatsMemoryUsageStatsOverhead();
  }

  public static String getKParamWatchDecisions() {
    return smlJNI.get_sml_Names_kParamWatchDecisions();
  }

  public static String getKParamWatchPhases() {
    return smlJNI.get_sml_Names_kParamWatchPhases();
  }

  public static String getKParamWatchProductionDefault() {
    return smlJNI.get_sml_Names_kParamWatchProductionDefault();
  }

  public static String getKParamWatchProductionUser() {
    return smlJNI.get_sml_Names_kParamWatchProductionUser();
  }

  public static String getKParamWatchProductionChunks() {
    return smlJNI.get_sml_Names_kParamWatchProductionChunks();
  }

  public static String getKParamWatchProductionJustifications() {
    return smlJNI.get_sml_Names_kParamWatchProductionJustifications();
  }

  public static String getKParamWatchWMEDetail() {
    return smlJNI.get_sml_Names_kParamWatchWMEDetail();
  }

  public static String getKParamWatchWorkingMemoryChanges() {
    return smlJNI.get_sml_Names_kParamWatchWorkingMemoryChanges();
  }

  public static String getKParamWatchPreferences() {
    return smlJNI.get_sml_Names_kParamWatchPreferences();
  }

  public static String getKParamWatchLearning() {
    return smlJNI.get_sml_Names_kParamWatchLearning();
  }

  public static String getKParamWatchBacktracing() {
    return smlJNI.get_sml_Names_kParamWatchBacktracing();
  }

  public static String getKParamWatchIndifferentSelection() {
    return smlJNI.get_sml_Names_kParamWatchIndifferentSelection();
  }

  public static String getKTrue() {
    return smlJNI.get_sml_Names_kTrue();
  }

  public static String getKFalse() {
    return smlJNI.get_sml_Names_kFalse();
  }

  public static String getKCommand_CreateAgent() {
    return smlJNI.get_sml_Names_kCommand_CreateAgent();
  }

  public static String getKCommand_DestroyAgent() {
    return smlJNI.get_sml_Names_kCommand_DestroyAgent();
  }

  public static String getKCommand_GetAgentList() {
    return smlJNI.get_sml_Names_kCommand_GetAgentList();
  }

  public static String getKCommand_LoadProductions() {
    return smlJNI.get_sml_Names_kCommand_LoadProductions();
  }

  public static String getKCommand_GetInputLink() {
    return smlJNI.get_sml_Names_kCommand_GetInputLink();
  }

  public static String getKCommand_GetOutputLink() {
    return smlJNI.get_sml_Names_kCommand_GetOutputLink();
  }

  public static String getKCommand_Run() {
    return smlJNI.get_sml_Names_kCommand_Run();
  }

  public static String getKCommand_Input() {
    return smlJNI.get_sml_Names_kCommand_Input();
  }

  public static String getKCommand_Output() {
    return smlJNI.get_sml_Names_kCommand_Output();
  }

  public static String getKCommand_CheckForIncomingCommands() {
    return smlJNI.get_sml_Names_kCommand_CheckForIncomingCommands();
  }

  public static String getKCommand_StopOnOutput() {
    return smlJNI.get_sml_Names_kCommand_StopOnOutput();
  }

  public static String getKCommand_RegisterForEvent() {
    return smlJNI.get_sml_Names_kCommand_RegisterForEvent();
  }

  public static String getKCommand_UnregisterForEvent() {
    return smlJNI.get_sml_Names_kCommand_UnregisterForEvent();
  }

  public static String getKCommand_Event() {
    return smlJNI.get_sml_Names_kCommand_Event();
  }

  public static String getKCommand_CommandLine() {
    return smlJNI.get_sml_Names_kCommand_CommandLine();
  }

  public static String getKCommand_ExpandCommandLine() {
    return smlJNI.get_sml_Names_kCommand_ExpandCommandLine();
  }

  public sml_Names() {
    this(smlJNI.new_sml_Names(), true);
  }

}
