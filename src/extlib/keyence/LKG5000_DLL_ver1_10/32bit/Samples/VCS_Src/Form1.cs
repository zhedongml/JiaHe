using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Collections.Generic;
using System.IO;

namespace Project1
{
    internal partial class Form1
        : System.Windows.Forms.Form
    {
        #region Constants

        private const int SW_SHOW = 5;
        private const int MAX_OUT = 12;
        private const int WAVEDATA_MAX = 2048;
        private const int PEEK_MAX = 4;
        private const int FILTER_PARAM_MAX = 10;
        private const int DISPLAYUNIT_MAX = 7;
        private object[] FILETER_PARAM_STRINGS = null;
        private object[] DISPLAY_UNIT_PARAM_STRINGS = null;

        #endregion Constants

        #region Methods

        [DllImport("wsock32.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static int inet_addr([MarshalAs(UnmanagedType.VBByRefStr)] ref string cp);

        public Form1()
            : base()
        {
            //This call is required by the Windows Form Designer.
            InitializeComponent();
            ReLoadForm(false);

            SetItemData(this.SyncParam, new int[] { 1, 0 });
            SetItemData(this.MultiParam2, new int[] { 1, 0 });
            SetItemData(this._SingleParam_1, new int[] { 1, 0 });
            SetItemData(this.SetProgramNoParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7 });
            SetItemData(this.SetPanelLockParam1, new int[] { 1, 0 });
            SetItemData(this.GetLightParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SingleParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetCalcDataSingleParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.DataStorageGetDataParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetMaskParam1_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.AbleStartParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetAbleModeParam_1, new int[] { 0, 1 });
            SetItemData(this.SetAbleMinMaxParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetAbleModeParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetReflectionModeParam_1, new int[] { 0, 1 });
            SetItemData(this._SetMeasureModeParam_1, new int[] { 0, 1, 2, 3, 4 });
            SetItemData(this.SetAlarmLevelParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetMeasureModeParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetReflectionModeParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.SetNumAlarmParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.SetToleranceParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetPanelParam_0, new int[] { -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetPanelParam_1, new int[] { -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetBasicPointParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetBasicPointParam_1, new int[] { 0, 1 });
            SetItemData(this.SetNumRecoveryParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetMedianParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetMaskParam1_1, new int[] { 1, 0 });
            SetItemData(this._SetMedianParam_1, new int[] { 0, 1, 2, 3 });
            SetItemData(this._SetLaserCtrlGroupParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetLaserCtrlGroupParam_1, new int[] { 0, 1 });
            SetItemData(this._SetRangeParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetRangeParam_1, new int[] { 0, 1 });
            SetItemData(this._SetMutualInterferencePreventionGroupParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetMutualInterferencePreventionGroupParam_1, new int[] { 0, 1, 2 });
            SetItemData(this._SetCalcMethodParam1_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetCalcMethodParam1_1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7 });
            SetItemData(this._SetOutAddSubParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetOutAddSubParam_1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetOutAddSubParam_2, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.SetOutOperationParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.SetScalingParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetCalcTargetParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetCalcTargetParam_1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 });
            SetItemData(this._SetFilterParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetFilterParam_1, new int[] { 0, 1, 2 });
            SetItemData(this._SetFilterParam_2, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 });
            SetItemData(this._SetTriggerModeParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetTriggerModeParam_1, new int[] { 0, 1 });
            SetItemData(this.SetOffsetParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetCalcModeParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetCalcModeParam_1, new int[] { 0, 1, 2, 3, 4, 5 });
            SetItemData(this.SetAnalogScalingParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetDisplayUnitParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetDisplayUnitParam_1, new int[] { 0, 1, 2, 3, 4, 5, 6 });
            SetItemData(this._SetMeasureTypeParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetMeasureTypeParam_1, new int[] { 0, 1, 2 });
            SetItemData(this._SetSynchronizationParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetSynchronizationParam_1, new int[] { 1, 0 });
            SetItemData(this._SetStorageTargetParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetStorageTargetParam_1, new int[] { 1, 0 });
            SetItemData(this.SetSamplingCycleParam, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8 });
            SetItemData(this.SetMutualInterferencePreventionParam, new int[] { 0, 1, 2 });
            SetItemData(this.SetStorobeTimeParam, new int[] { 0, 1, 2, 3 });
            SetItemData(this.SetDataStorageParam2, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });
            SetItemData(this._SetAnalogChannelParam_0, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this._SetAnalogChannelParam_1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.SetAlarmOutputFormParam, new int[] { 0, 1, 2 });
            SetItemData(this.SetToleranceComparatorOutputFormatParam, new int[] { 0, 1, 2 });
            SetItemData(this.GetToleranceParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetAbleModeParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetAbleMinMaxParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetMeasureModeParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetNumAlarmParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetBasicPointParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetNumRecoveryParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetAlarmLevelParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetReflectionModeParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetMaskParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetMedianParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetLaserCtrlGroupParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetRangeParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetMutualInterferencePreventionGroupParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetCalcMethodParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetCalcTargetParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetOutAddSubParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetOutOperationParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetScalingParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetAnalogChannelParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetStorageTargetParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetSynchronizationParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetMeasureTypeParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetDisplayUnitParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetAnalogScalingParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetCalcModeParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetOffsetParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetTriggerModeParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
            SetItemData(this.GetFilterParam1, new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
        }

        private void ResetListForDisplayUnitParam()
        {
            int measureType = SetMeasureTypeParam[1].SelectedIndex;
            for (int cnt = 0; cnt <= DISPLAYUNIT_MAX - 1; cnt++)
            {
                SetListItem(SetDisplayUnitParam[1],cnt, Convert.ToString(((Array)DISPLAY_UNIT_PARAM_STRINGS[measureType]).GetValue(cnt)));
            }
        }

        private string GetDisplayUnitParamString(LKIF2.LKIF_MEASURETYPE measureType, LKIF2.LKIF_DISPLAYUNIT displayUnitParam)
        {
            return Convert.ToString(((Array)DISPLAY_UNIT_PARAM_STRINGS[(int)measureType]).GetValue((int)displayUnitParam));
        }

        private void ResetListForFileterParam()
        {
            int filterMode = 0;
            KeyValuePair<string, int> item = (KeyValuePair<string, int>)SetFilterParam[1].SelectedItem;
            if ( item.Value > 0)
            {
                filterMode = 1;
            }

            for (int cnt = 0; cnt <= FILTER_PARAM_MAX - 1; cnt++)
            {
                SetListItem(SetFilterParam[2],cnt, Convert.ToString(((Array)FILETER_PARAM_STRINGS[filterMode]).GetValue(cnt)));
            }
        }

        private string GetFileterParamString(LKIF2.LKIF_FILTERMODE filterMode, LKIF2.LKIF_FILTERPARA filterParam)
        {

            if (filterMode > LKIF2.LKIF_FILTERMODE.LKIF_FILTERMODE_LOWPASS)
            {
                filterMode = LKIF2.LKIF_FILTERMODE.LKIF_FILTERMODE_LOWPASS;
            }

            return Convert.ToString(((Array)FILETER_PARAM_STRINGS[(int)filterMode]).GetValue((int)filterParam));
        }

        private void FuncQuit(string funcName, LKIF2.RC returnCode)
        {
            if (IsSuccess(returnCode))
            {
                ShowSuccessMsg(funcName, returnCode);
            }
            else
            {
                ShowFailureMsg(funcName, returnCode);
            }
        }

        private bool IsSuccess(LKIF2.RC returnCode)
        {
            bool result = false;
            if (returnCode == LKIF2.RC.RC_OK)
            {
                result = true;
            }
            return result;
        }

        private void ShowSuccessMsg(string funcName, LKIF2.RC returnCode)
        {
            ResultText.Text = funcName + " is succeeded. RC = 0x" + ((int)returnCode).ToString("X");
        }

        private void ShowFailureMsg(string funcName, LKIF2.RC returnCode)
        {
            string MessageText = "Failed in " + funcName + ". RC = 0x" + ((int)returnCode).ToString("X");
            if (IsDisplayMessageBox.CheckState == CheckState.Checked)
            {
                MessageBox.Show(MessageText, Application.ProductName);
            }
            ResultText.Text = MessageText;
        }

        private LKIF2.LKIF_OUTNO MakeOutNoFromCheckBoxes(CheckBox[] checkBoxes)
        {
            LKIF2.LKIF_OUTNO result = (LKIF2.LKIF_OUTNO)0;
            LKIF2.LKIF_OUTNO[] OutNoTable = new LKIF2.LKIF_OUTNO[] { LKIF2.LKIF_OUTNO.LKIF_OUTNO_01, LKIF2.LKIF_OUTNO.LKIF_OUTNO_02, LKIF2.LKIF_OUTNO.LKIF_OUTNO_03,
                                                                     LKIF2.LKIF_OUTNO.LKIF_OUTNO_04, LKIF2.LKIF_OUTNO.LKIF_OUTNO_05, LKIF2.LKIF_OUTNO.LKIF_OUTNO_06, 
                                                                     LKIF2.LKIF_OUTNO.LKIF_OUTNO_07, LKIF2.LKIF_OUTNO.LKIF_OUTNO_08, LKIF2.LKIF_OUTNO.LKIF_OUTNO_09,
                                                                     LKIF2.LKIF_OUTNO.LKIF_OUTNO_10, LKIF2.LKIF_OUTNO.LKIF_OUTNO_11, LKIF2.LKIF_OUTNO.LKIF_OUTNO_12 };

            int index = 0;
            foreach (CheckBox checkBox in checkBoxes)
            {
                if (checkBox.Checked)
                {
                    result = result | OutNoTable[index];
                }
                index++;
            }
            return result;
        }

        private void SetCheckBoxesForOutNo(CheckBox[] checkBoxes, LKIF2.LKIF_OUTNO outNo)
        {
            LKIF2.LKIF_OUTNO[] OutNoTable = new LKIF2.LKIF_OUTNO[] { LKIF2.LKIF_OUTNO.LKIF_OUTNO_01, LKIF2.LKIF_OUTNO.LKIF_OUTNO_02, LKIF2.LKIF_OUTNO.LKIF_OUTNO_03, 
                                                                     LKIF2.LKIF_OUTNO.LKIF_OUTNO_04, LKIF2.LKIF_OUTNO.LKIF_OUTNO_05, LKIF2.LKIF_OUTNO.LKIF_OUTNO_06, 
                                                                     LKIF2.LKIF_OUTNO.LKIF_OUTNO_07, LKIF2.LKIF_OUTNO.LKIF_OUTNO_08, LKIF2.LKIF_OUTNO.LKIF_OUTNO_09, 
                                                                     LKIF2.LKIF_OUTNO.LKIF_OUTNO_10, LKIF2.LKIF_OUTNO.LKIF_OUTNO_11, LKIF2.LKIF_OUTNO.LKIF_OUTNO_12 };

            int index = 0;
            foreach (CheckBox checkBox in checkBoxes)
            {
                if ((outNo & OutNoTable[index]) == OutNoTable[index])
                {
                    checkBox.CheckState = CheckState.Checked;
                }
                else
                {
                    checkBox.CheckState = CheckState.Unchecked;
                }
                index++;
            }
        }

        private int CountCheckFromCheckBoxes(CheckBox[] checkBoxes)
        {
            int result = 0;
            LKIF2.LKIF_OUTNO[] OutNoTable = new LKIF2.LKIF_OUTNO[12];

            foreach (CheckBox checkBox in checkBoxes)
            {
                if (checkBox.CheckState == CheckState.Checked)
                {
                    result++;
                }
            }
            return result;
        }

        private string FloatResultValueOutToText(LKIF2.LKIF_FLOATVALUE_OUT FloatValue)
        {
            if (FloatValue.FloatResult == LKIF2.LKIF_FLOATRESULT.LKIF_FLOATRESULT_VALID)
            {
                return FloatValue.value.ToString();
            }
            else if (FloatValue.FloatResult == LKIF2.LKIF_FLOATRESULT.LKIF_FLOATRESULT_RANGEOVER_P)
            {
                return "+FFFFFFF";
            }
            else if (FloatValue.FloatResult == LKIF2.LKIF_FLOATRESULT.LKIF_FLOATRESULT_RANGEOVER_N)
            {
                return "-FFFFFFF";
            }
            else if (FloatValue.FloatResult == LKIF2.LKIF_FLOATRESULT.LKIF_FLOATRESULT_WAITING)
            {
                return "--------";
            }
            else if (FloatValue.FloatResult == LKIF2.LKIF_FLOATRESULT.LKIF_FLOATRESULT_ALARM)
            {
                return "alarm";
            }
            else
            {
                return "Invalid";
            }
        }
        private string FloatResultValueToText(LKIF2.LKIF_FLOATVALUE FloatValue)
        {
            if (FloatValue.FloatResult == LKIF2.LKIF_FLOATRESULT.LKIF_FLOATRESULT_VALID)
            {
                return "LKIF_FLOATRESULT_VALID";
            }
            else if (FloatValue.FloatResult == LKIF2.LKIF_FLOATRESULT.LKIF_FLOATRESULT_RANGEOVER_P)
            {
                return "LKIF_FLOATRESULT_RANGEOVER_P";
            }
            else if (FloatValue.FloatResult == LKIF2.LKIF_FLOATRESULT.LKIF_FLOATRESULT_RANGEOVER_N)
            {
                return "LKIF_FLOATRESULT_RANGEOVER_N";
            }
            else if (FloatValue.FloatResult == LKIF2.LKIF_FLOATRESULT.LKIF_FLOATRESULT_WAITING)
            {
                return "LKIF_FLOATRESULT_WAITING";
            }
            else if (FloatValue.FloatResult == LKIF2.LKIF_FLOATRESULT.LKIF_FLOATRESULT_ALARM)
            {
                return "LKIF_FLOATRESULT_ALARM";
            }
            else
            {
                return "LKIF_FLOATRESULT_INVALID";
            }
        }
        private bool GetTextData(TextBox txt, ref int value)
        {
            bool result = false;
            try
            {
                result = true;
                value = Convert.ToInt32(Double.Parse(txt.Text));
                return result;
            }
            catch
            {
                MessageBox.Show("illegal input", Application.ProductName);
                return false;
            }
        }

        private bool GetComboData(ComboBox cmb, ref int value)
        {
            try
            {
                if (cmb.SelectedIndex == -1)
                {
                    value = Convert.ToInt32(Double.Parse(cmb.Text));
                }
                else
                {
                    KeyValuePair<string, int> item = (KeyValuePair<string, int>)cmb.SelectedItem;
                    value = item.Value;
                }
                return true;
            }
            catch
            {
                MessageBox.Show("illegal input", Application.ProductName);
                return false;
            }
        }

        private void CloseButton_Click(Object eventSender, EventArgs eventArgs)
        {
            this.Close();
        }

        private void Form_Initialize()
        {
            DISPLAY_UNIT_PARAM_STRINGS = new object[] { new object[] { "LKIF_DISPLAYUNIT_0000_01MM", "LKIF_DISPLAYUNIT_000_001MM", "LKIF_DISPLAYUNIT_00_0001MM",
                                                                       "LKIF_DISPLAYUNIT_0_00001MM", "LKIF_DISPLAYUNIT_00000_1UM", "LKIF_DISPLAYUNIT_0000_01UM", 
                                                                       "LKIF_DISPLAYUNIT_000_001UM" }, 
                                                        new object[] { "LKIF_DISPLAYUNIT_00000_1M_S", "LKIF_DISPLAYUNIT_0000_01M_S", "LKIF_DISPLAYUNIT_000_001M_S", 
                                                                       "LKIF_DISPLAYUNIT_00000_1MM_S", "LKIF_DISPLAYUNIT_0000_01MM_S", "LKIF_DISPLAYUNIT_000_001MM_S", 
                                                                       "LKIF_DISPLAYUNIT_00_0001MM_S" }, 
                                                        new object[] { "LKIF_DISPLAYUNIT_00000_1KM_S2", "LKIF_DISPLAYUNIT_0000_01KM_S2", "LKIF_DISPLAYUNIT_000_001KM_S2",
                                                                       "LKIF_DISPLAYUNIT_00000_1M_S2", "LKIF_DISPLAYUNIT_0000_01M_S2", "LKIF_DISPLAYUNIT_000_001M_S2", 
                                                                       "LKIF_DISPLAYUNIT_00_0001M_S2" } };
            FILETER_PARAM_STRINGS = new object[] { new object[] { "LKIF_FILTERPARA_AVE_1", "LKIF_FILTERPARA_AVE_4", "LKIF_FILTERPARA_AVE_16",
                                                                  "LKIF_FILTERPARA_AVE_64", "LKIF_FILTERPARA_AVE_256", "LKIF_FILTERPARA_AVE_1024", 
                                                                  "LKIF_FILTERPARA_AVE_4096", "LKIF_FILTERPARA_AVE_16384", "LKIF_FILTERPARA_AVE_65536",
                                                                  "LKIF_FILTERPARA_AVE_262144" }, 
                                                   new object[] { "LKIF_FILTERPARA_COFF_3000", "LKIF_FILTERPARA_COFF_1000", "LKIF_FILTERPARA_COFF_300",
                                                                  "LKIF_FILTERPARA_COFF_100", "LKIF_FILTERPARA_COFF_30", "LKIF_FILTERPARA_COFF_10",
                                                                  "LKIF_FILTERPARA_COFF_3", "LKIF_FILTERPARA_COFF_1", "LKIF_FILTERPARA_COFF_0_3", 
                                                                  "LKIF_FILTERPARA_COFF_0_1" } };
        }

        private void Form_Load()
        {
            ComboBox cmb = null;

            foreach (Control ctl in Controls)
            {
                cmb = ctl as ComboBox;
                if (cmb != null)
                {
                    cmb.SelectedIndex = 0;
                }
            }

            SetAbleMinMaxParam2[0].Text = "1";
            SetAbleMinMaxParam2[1].Text = "99";
            SetNumAlarmParam2.Text = "8";
            SetAlarmLevelParam2.Text = "4";
            SetDataStorageParam1.Text = "100000";
            SetToleranceParam2[0].Text = "50000";
            SetToleranceParam2[1].Text = "-50000";
            SetToleranceParam2[2].Text = "0";
            SetScalingParam2[0].Text = "0";
            SetScalingParam2[1].Text = "0";
            SetScalingParam2[2].Text = "1000";
            SetScalingParam2[3].Text = "1000";
            SetFilterParam[2].SelectedIndex = 4;
            SetAnalogScalingParam2[0].Text = "1000";
            SetAnalogScalingParam2[1].Text = "1000";
            SetAnalogScalingParam2[2].Text = "-1000";
            SetAnalogScalingParam2[3].Text = "-1000";
            SetOffsetParam2.Text = "0";
            SetDisplayUnitParam[1].SelectedIndex = 1;
            SetPanelLockParam1.SelectedIndex = 1;
            SetNumOfUsedHeadsParam.Text = "2";
            SetNumOfUsedOutParam.Text = "2";
            SetNumOfUsedAnalogChParam.Text = "2";
            SetSamplingCycleParam.SelectedIndex = 6;
            SetOutAddSubParam[1].SelectedIndex = 1;
            SetOutAddSubParam[2].SelectedIndex = 1;
            SetMaskParam1[1].SelectedIndex = 1;
            SetPanelParam[0].SelectedIndex = 1;
            SetPanelParam[1].SelectedIndex = 2;

			GetCalcDataSingleParam1.SelectedIndex = 0;
			_SingleParam_0.SelectedIndex = 0;
			_SingleParam_1.SelectedIndex = 1;
			MultiParam2.SelectedIndex = 1;
			SyncParam.SelectedIndex = 1;
			SetProgramNoParam1.SelectedIndex = 0;
			DataStorageGetDataParam1.SelectedIndex = 0;
			GetLightParam1.SelectedIndex = 0;

			//_SetPanelParam_0.SelectedIndex = 0;
			//_SetPanelParam_1.SelectedIndex = 0;
			SetToleranceParam1.SelectedIndex = 0;
			_SetAbleModeParam_0.SelectedIndex = 0;
			_SetAbleModeParam_1.SelectedIndex = 0;
			SetAbleMinMaxParam1.SelectedIndex = 0;
			_SetMeasureModeParam_0.SelectedIndex = 0;
			_SetMeasureModeParam_1.SelectedIndex = 0;
			_SetBasicPointParam_0.SelectedIndex = 0;
			_SetBasicPointParam_1.SelectedIndex = 0;
			SetNumAlarmParam1.SelectedIndex = 0;
			SetNumRecoveryParam1.SelectedIndex = 0;
			SetAlarmLevelParam1.SelectedIndex = 0;
			AbleStartParam1.SelectedIndex = 0;
			_SetReflectionModeParam_0.SelectedIndex = 0;
			_SetReflectionModeParam_1.SelectedIndex = 0;
			_SetMaskParam1_0.SelectedIndex = 0;
			_SetMedianParam_0.SelectedIndex = 0;
			_SetMedianParam_1.SelectedIndex = 0;
			_SetLaserCtrlGroupParam_0.SelectedIndex = 0;
			_SetLaserCtrlGroupParam_1.SelectedIndex = 0;
			_SetRangeParam_0.SelectedIndex = 0;
			_SetRangeParam_1.SelectedIndex = 0;
			_SetMutualInterferencePreventionGroupParam_0.SelectedIndex = 0;
			_SetMutualInterferencePreventionGroupParam_1.SelectedIndex = 0;
			_SetCalcMethodParam1_0.SelectedIndex = 0;
			_SetCalcMethodParam1_1.SelectedIndex = 0;
			_SetCalcTargetParam_0.SelectedIndex = 0;
			_SetCalcTargetParam_1.SelectedIndex = 0;
			_SetOutAddSubParam_0.SelectedIndex = 0;
			SetOutOperationParam1.SelectedIndex = 0;
			SetScalingParam1.SelectedIndex = 0;

			_SetFilterParam_0.SelectedIndex = 0;
			_SetFilterParam_1.SelectedIndex = 0;
			_SetTriggerModeParam_0.SelectedIndex = 0;
			_SetTriggerModeParam_1.SelectedIndex = 0;
			SetOffsetParam1.SelectedIndex = 0;
			_SetCalcModeParam_0.SelectedIndex = 0;
			_SetCalcModeParam_1.SelectedIndex = 0;
			SetAnalogScalingParam1.SelectedIndex = 0;
			_SetDisplayUnitParam_0.SelectedIndex = 0;
			_SetMeasureTypeParam_0.SelectedIndex = 0;
			_SetMeasureTypeParam_1.SelectedIndex = 0;
			_SetSynchronizationParam_0.SelectedIndex = 0;
			_SetSynchronizationParam_1.SelectedIndex = 1;
			_SetStorageTargetParam_0.SelectedIndex = 0;
			_SetStorageTargetParam_1.SelectedIndex = 1;
			SetMutualInterferencePreventionParam.SelectedIndex = 0;
			SetToleranceComparatorOutputFormatParam.SelectedIndex = 0;
			SetStorobeTimeParam.SelectedIndex = 0;
			SetDataStorageParam2.SelectedIndex = 0;
			_SetAnalogChannelParam_0.SelectedIndex = 0;
			_SetAnalogChannelParam_1.SelectedIndex = 0;
			SetAlarmOutputFormParam.SelectedIndex = 0;

			GetToleranceParam1.SelectedIndex = 0;
			GetAbleModeParam1.SelectedIndex = 0;
			GetAbleMinMaxParam1.SelectedIndex = 0;
			GetMeasureModeParam1.SelectedIndex = 0;
			GetBasicPointParam1.SelectedIndex = 0;
			GetNumAlarmParam1.SelectedIndex = 0;
			GetNumRecoveryParam1.SelectedIndex = 0;
			GetAlarmLevelParam1.SelectedIndex = 0;
			GetReflectionModeParam1.SelectedIndex = 0;
			GetMaskParam1.SelectedIndex = 0;
			GetMedianParam1.SelectedIndex = 0;
			GetLaserCtrlGroupParam1.SelectedIndex = 0;
			GetRangeParam1.SelectedIndex = 0;
			GetMutualInterferencePreventionGroupParam1.SelectedIndex = 0;
			GetCalcMethodParam1.SelectedIndex = 0;
			GetCalcTargetParam1.SelectedIndex = 0;
			GetOutAddSubParam1.SelectedIndex = 0;
			GetOutOperationParam1.SelectedIndex = 0;
			GetScalingParam1.SelectedIndex = 0;

			GetFilterParam1.SelectedIndex = 0;
			GetTriggerModeParam1.SelectedIndex = 0;
			GetOffsetParam1.SelectedIndex = 0;
			GetCalcModeParam1.SelectedIndex = 0;
			GetAnalogScalingParam1.SelectedIndex = 0;
			GetDisplayUnitParam1.SelectedIndex = 0;
			GetMeasureTypeParam1.SelectedIndex = 0;
			GetSynchronizationParam1.SelectedIndex = 0;
			GetStorageTargetParam1.SelectedIndex = 0;
			GetAnalogChannelParam1.SelectedIndex = 0;
        }

        private void GetCalcData_Click(Object eventSender, EventArgs eventArgs)
        {
            int Index = Array.IndexOf(GetCalcData, eventSender);
            switch (Index)
            {
                case 0:
                    GetCalcDataSimple();
                    break;
                case 1:
                    GetCalcDataMulti();
                    break;
                case 2:
                    GetCalcDataAll();
                    break;
            }
        }

        private void GetCalcDataSimple()
        {
            int outNo = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            LKIF2.LKIF_FLOATVALUE_OUT calcData = new LKIF2.LKIF_FLOATVALUE_OUT();

            if (GetComboData(GetCalcDataSingleParam1, ref outNo))
            {
                result = LKIF2.LKIF2_GetCalcDataSingle(outNo, ref calcData);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetCalcDataSingleParam2.Text = FloatResultValueOutToText(calcData);
            }

            FuncQuit("LKIF2_GetCalcDataSingle", result);
        }

        private void GetCalcDataMulti()
        {
            LKIF2.LKIF_FLOATVALUE_OUT[] calcData = new LKIF2.LKIF_FLOATVALUE_OUT[MAX_OUT + 1];

            LKIF2.LKIF_OUTNO outNo = MakeOutNoFromCheckBoxes(GetCalcDataMultiParam1);
            LKIF2.RC result = LKIF2.LKIF2_GetCalcDataMulti(outNo, ref calcData[0]);

            if (IsSuccess(result))
            {
                for (int cnt = 0; cnt <= CountCheckFromCheckBoxes(GetCalcDataMultiParam1) - 1; cnt++)
                {
                    GetCalcDataMultiParam2[calcData[cnt].outNo].Text = FloatResultValueOutToText(calcData[cnt]);
                }
            }

            FuncQuit("LKIF2_GetCalcDataMulti", result);
        }

        private void GetCalcDataAll()
        {
            LKIF2.LKIF_FLOATVALUE_OUT[] calcData = new LKIF2.LKIF_FLOATVALUE_OUT[MAX_OUT + 1] ;
            int maxOut = 0;

            LKIF2.RC result = LKIF2.LKIF2_GetCalcDataALL(ref maxOut, ref calcData[0]);

            if (IsSuccess(result))
            {
                for (int cnt = 0; cnt <= MAX_OUT - 1; cnt++)
                {
                    if (cnt < maxOut)
                    {
                        GetCalcDataAllParam2[cnt].Text = FloatResultValueOutToText(calcData[cnt]);
                    }
                    else
                    {
                        GetCalcDataAllParam2[cnt].Text = "------";
                    }
                }
            }
            FuncQuit("LKIF2_GetCalcDataALL", result);
        }

        private void SetFilterParam_SelectedIndexChanged(Object eventSender, EventArgs eventArgs)
        {
            int Index = Array.IndexOf(SetFilterParam, eventSender);
            if (Index == 1)
            {
                ResetListForFileterParam();
            }
        }

        private void SetMeasureTypeParam_SelectedIndexChanged(Object eventSender, EventArgs eventArgs)
        {
            int Index = Array.IndexOf(SetMeasureTypeParam, eventSender);
            if (Index == 1)
            {
                ResetListForDisplayUnitParam();
            }
        }

        private void SetTiming_Click(Object eventSender, EventArgs eventArgs)
        {
            int Index = Array.IndexOf(SetTiming, eventSender);
            switch (Index)
            {
                case 0:
                    SetTimingSingle();
                    break;
                case 1:
                    SetTimingMulti();
                    break;
                case 2:
                    SetTimingSync();
                    break;
            }
        }

        private void SetTimingSingle()
        {
            int outNo = 0;
            int onOff = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(SingleParam[0], ref outNo) && GetComboData(SingleParam[1], ref onOff))
            {
                result = LKIF2.LKIF2_SetTimingSingle(outNo, onOff);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_SetTimingSingle", result);
        }

        private void SetTimingMulti()
        {
            int onOff = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            LKIF2.LKIF_OUTNO outNo = MakeOutNoFromCheckBoxes(MultiParam1);

            if (GetComboData(MultiParam2, ref onOff))
            {
                result = LKIF2.LKIF2_SetTimingMulti(outNo, onOff);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_SetTimingMulti", result);
        }

        private void SetTimingSync()
        {
            LKIF2.RC result = (LKIF2.RC)0;
            int onOff = 0;
            if (GetComboData(SyncParam, ref onOff))
            {
                result = LKIF2.LKIF2_SetTimingSync(onOff);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_SetTimingSync", result);
        }

        private void SetZero_Click(Object eventSender, EventArgs eventArgs)
        {
            int Index = Array.IndexOf(SetZero, eventSender);
            switch (Index)
            {
                case 0:
                    SetZeroSingle();
                    break;
                case 1:
                    SetZeroMulti();
                    break;
                case 2:
                    SetZeroSync();
                    break;
            }
        }

        private void SetZeroSingle()
        {
            int outNo = 0;
            int onOff = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(SingleParam[0], ref outNo) && GetComboData(SingleParam[1], ref onOff))
            {
                result = LKIF2.LKIF2_SetZeroSingle(outNo, onOff);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_SetZeroSingle", result);
        }

        private void SetZeroMulti()
        {
            int onOff = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            LKIF2.LKIF_OUTNO outNo = MakeOutNoFromCheckBoxes(MultiParam1);

            if (GetComboData(MultiParam2, ref onOff))
            {
                result = LKIF2.LKIF2_SetZeroMulti(outNo, onOff);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_SetZeroMulti", result);
        }

        private void SetZeroSync()
        {
            LKIF2.RC result = (LKIF2.RC)0;
            int onOff = 0;

            if (GetComboData(SyncParam, ref onOff))
            {
                result = LKIF2.LKIF2_SetZeroSync(onOff);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_SetZeroSync", result);
        }

        private void SetReset_Click(Object eventSender, EventArgs eventArgs)
        {
            int Index = Array.IndexOf(SetReset, eventSender);
            switch (Index)
            {
                case 0:
                    SetResetSingle();
                    break;
                case 1:
                    SetResetMulti();
                    break;
                case 2:
                    SetResetSync();
                    break;
            }
        }

        private void SetResetSingle()
        {
            int outNo = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(SingleParam[0], ref outNo))
            {
                result = LKIF2.LKIF2_SetResetSingle(outNo);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_SetResetSingle", result);
        }

        private void SetResetMulti()
        {
            LKIF2.LKIF_OUTNO outNo = MakeOutNoFromCheckBoxes(MultiParam1);
            LKIF2.RC result = LKIF2.LKIF2_SetResetMulti(outNo);

            FuncQuit("LKIF2_SetResetMulti", result);
        }

        private void SetResetSync()
        {
            FuncQuit("LKIF2_SetResetSync", LKIF2.LKIF2_SetResetSync());
        }

        private void SetPanelLock_Click(Object eventSender, EventArgs eventArgs)
        {
            LKIF2.RC result = (LKIF2.RC)0;
            int onOff = 0;

            if (GetComboData(SetPanelLockParam1, ref onOff))
            {
                result = LKIF2.LKIF2_SetPanelLock(onOff);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_SetPanelLock", result);
        }

        private void SetProgramNo_Click(Object eventSender, EventArgs eventArgs)
        {
            LKIF2.RC result = (LKIF2.RC)0;
            int programNo = 0;

            if (GetComboData(SetProgramNoParam1, ref programNo))
            {
                result = LKIF2.LKIF2_SetProgramNo(programNo);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_SetProgramNo", result);
        }

        private void GetProgramNo_Click(Object eventSender, EventArgs eventArgs)
        {
            int programNo = 0;

            LKIF2.RC result = LKIF2.LKIF2_GetProgramNo(ref programNo);

            if (IsSuccess(result))
            {
                GetProgramNoParam1.Text = GetListText(SetProgramNoParam1,programNo);
            }

            FuncQuit("LKIF2_GetProgramNo", result);
        }

        private void DataStorageStart_Click(Object eventSender, EventArgs eventArgs)
        {
            FuncQuit("LKIF2_DataStorageStart", LKIF2.LKIF2_DataStorageStart());
        }

        private void DataStorageStop_Click(Object eventSender, EventArgs eventArgs)
        {
            FuncQuit("LKIF2_DataStorageStop", LKIF2.LKIF2_DataStorageStop());
        }

        private void DataStorageInit_Click(Object eventSender, EventArgs eventArgs)
        {
            FuncQuit("LKIF2_DataStorageInit", LKIF2.LKIF2_DataStorageInit());
        }

        private void DataStorageGetData_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            int numOutBuffer = 0;
            LKIF2.LKIF_FLOATVALUE[] OutBuffer = new LKIF2.LKIF_FLOATVALUE[1200001];
            int numReceived = 0;

            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(DataStorageGetDataParam1, ref outNo) && GetTextData(DataStorageGetDataParam2, ref numOutBuffer))
            {
                result = LKIF2.LKIF2_DataStorageGetData(outNo, numOutBuffer, ref OutBuffer[0], ref numReceived);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                DataStorageGetDataParam3.Text = numReceived.ToString();

                if (checkSaveDataStorageGetData.Checked)
                {
                    if (CommonDialogForDataStorageSave.ShowDialog() != DialogResult.Cancel)
                    {
                        try
                        {
                            StreamWriter writer = new StreamWriter(CommonDialogForDataStorageSave.FileName);
                            writer.WriteLine("OutNo");
                            writer.WriteLine(outNo);
                            writer.WriteLine("Data,FloatResult");
                            for (int cnt = 0; cnt <= numReceived - 1; cnt++)
                            {
                                writer.WriteLine(OutBuffer[cnt].value.ToString() + "," + FloatResultValueToText(OutBuffer[cnt]));
                            }
                            writer.Close();
                        }
                        catch
                        {
                            MessageBox.Show("Failed in saving the File.", Application.ProductName);
                        }
                    }
                }
            }
            FuncQuit("LKIF2_DataStorageGetStatus", result);
        }

        private void DataStorageGetStatus_Click(Object eventSender, EventArgs eventArgs)
        {
            int isStorage = 0;
            int numStorageData = 0;
            string[] StorageMessage = new string[] { "Stopping in progress", "Storage in progress" };

            LKIF2.RC result = LKIF2.LKIF2_DataStorageGetStatus(ref isStorage, ref numStorageData);

            if (IsSuccess(result))
            {
                DataStorageGetStatusParam1.Text = StorageMessage[isStorage];
                DataStorageGetStatusParam2.Text = numStorageData.ToString();
            }

            FuncQuit("LKIF2_DataStorageGetStatus", result);
        }

        private void GetLight_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            int peekNo = 0;
            int[] measurePosition = new int[PEEK_MAX + 1];
            byte[] waveData = new byte[WAVEDATA_MAX + 1];

            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(GetLightParam1, ref headNo) && GetTextData(GetLightParam2, ref peekNo))
            {
                result = LKIF2.LKIF2_GetLight(headNo, peekNo, ref measurePosition[0], ref waveData[0]);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                if (checkSaveGetLight.Checked)
                {
                    if (CommonDialogForLightSave.ShowDialog() != DialogResult.Cancel)
                    {
                        try
                        {
                            StreamWriter writer = new StreamWriter(CommonDialogForLightSave.FileName);
                            writer.WriteLine("HeadNo," + waveData[0].ToString());
                            writer.WriteLine(headNo.ToString() + "," + waveData[1].ToString());
                            writer.WriteLine("PeekNo," + waveData[2].ToString());
                            writer.WriteLine(peekNo.ToString() + "," + waveData[3].ToString());
                            writer.WriteLine("MeasurePosition," + waveData[4].ToString());
                            writer.WriteLine(measurePosition[0].ToString() + "," + waveData[5].ToString());
                            writer.WriteLine(measurePosition[1].ToString() + "," + waveData[6].ToString());
                            writer.WriteLine(measurePosition[2].ToString() + "," + waveData[7].ToString());
                            writer.WriteLine( measurePosition[3].ToString() + "," + waveData[8].ToString());
                            for (int cnt = 9; cnt <= WAVEDATA_MAX - 1; cnt++)
                            {
                                writer.WriteLine("," + waveData[cnt].ToString());
                            }
                            writer.Close();
                        }
                        catch
                        {
                            MessageBox.Show("Failed in saving the File.", Application.ProductName);
                        }
                    }
                }
            }
            FuncQuit("LKIF2_GetLight", result);
        }

        private void SetPanel_Click(Object eventSender, EventArgs eventArgs)
        {
            int upperDisp = 0;
            int lowerDisp = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(SetPanelParam[0], ref upperDisp) && GetComboData(SetPanelParam[1], ref lowerDisp))
            {
                result = LKIF2.LKIF2_SetPanel(upperDisp, lowerDisp);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_SetPanel", result);
        }

        private void SetTolerance_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            int upperLimit = 0;
            int lowerLimit = 0;
            int hystersis = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(SetToleranceParam1, ref outNo) && GetTextData(SetToleranceParam2[0], ref upperLimit) &&
                GetTextData(SetToleranceParam2[1], ref lowerLimit) && GetTextData(SetToleranceParam2[2], ref hystersis))
            {
                result = LKIF2.LKIF2_SetTolerance(outNo, upperLimit, lowerLimit, hystersis);
            }
            else
            {
                return;
            }
            FuncQuit("LKIF2_SetTolerance", result);

        }

        private void SetAbleMode_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_ABLEMODE ableMode = (LKIF2.LKIF_ABLEMODE)0;
            LKIF2.RC result = (LKIF2.RC)0;
            int tempRefParam = (int)ableMode;
            if (GetComboData(SetAbleModeParam[0], ref headNo) && GetComboData(SetAbleModeParam[1], ref tempRefParam))
            {
                ableMode = (LKIF2.LKIF_ABLEMODE)tempRefParam;
                result = LKIF2.LKIF2_SetAbleMode(headNo, ableMode);
            }
            else
            {
                ableMode = (LKIF2.LKIF_ABLEMODE)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetAbleMode", result);
        }

        private void SetAbleMinMax_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            int min = 0;
            int max = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(SetAbleMinMaxParam1, ref headNo) && GetTextData(SetAbleMinMaxParam2[0], ref min) && GetTextData(SetAbleMinMaxParam2[1], ref max))
            {
                result = LKIF2.LKIF2_SetAbleMinMax(headNo, min, max);
            }
            else
            {
                return;
            }
            FuncQuit("LKIF2_SetAbleMinMax", result);
        }

        private void SetMeasureMode_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_MEASUREMODE mesureMode = (LKIF2.LKIF_MEASUREMODE)0;
            LKIF2.RC result = (LKIF2.RC)0;
            int tempRefParam = (int)mesureMode;
            if (GetComboData(SetMeasureModeParam[0], ref headNo) && GetComboData(SetMeasureModeParam[1], ref tempRefParam))
            {
                mesureMode = (LKIF2.LKIF_MEASUREMODE)tempRefParam;
                result = LKIF2.LKIF2_SetMeasureMode(headNo, mesureMode);
            }
            else
            {
                mesureMode = (LKIF2.LKIF_MEASUREMODE)tempRefParam;
                return;
            }

            FuncQuit("LKIF2_SetMeasureMode", result);
        }

        private void SetBasicPoint_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_BASICPOINT basicPoint = (LKIF2.LKIF_BASICPOINT)0;
            LKIF2.RC result = (LKIF2.RC)0;
            int tempRefParam = (int)basicPoint;
            if (GetComboData(SetBasicPointParam[0], ref headNo) && GetComboData(SetBasicPointParam[1], ref tempRefParam))
            {
                basicPoint = (LKIF2.LKIF_BASICPOINT)tempRefParam;
                result = LKIF2.LKIF2_SetBasicPoint(headNo, basicPoint);
            }
            else
            {
                basicPoint = (LKIF2.LKIF_BASICPOINT)tempRefParam;
                return;
            }

            FuncQuit("LKIF2_SetBasicPoint", result);
        }

        private void SetNumAlarm_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            int numAlarm = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(SetNumAlarmParam1, ref headNo) && GetTextData(SetNumAlarmParam2, ref numAlarm))
            {
                result = LKIF2.LKIF2_SetNumAlarm(headNo, numAlarm);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_SetNumAlarm", result);
        }

        private void SetNumRecovery_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            int numRecovery = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(SetNumRecoveryParam1, ref headNo) && GetTextData(SetNumRecoveryParam2, ref numRecovery))
            {
                result = LKIF2.LKIF2_SetNumRecovery(headNo, numRecovery);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_SetNumRecovery", result);
        }

        private void SetAlarmLevel_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            int alaramLevel = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(SetAlarmLevelParam1, ref headNo) && GetTextData(SetAlarmLevelParam2, ref alaramLevel))
            {
                result = LKIF2.LKIF2_SetAlarmLevel(headNo, alaramLevel);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_SetAlarmLevel", result);
        }

        private void AbleStart_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(AbleStartParam1, ref headNo))
            {
                result = LKIF2.LKIF2_AbleStart(headNo);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_AbleStart", result);
        }

        private void AbleCancel_Click(Object eventSender, EventArgs eventArgs)
        {
            FuncQuit("LKIF2_AbleCancel", LKIF2.LKIF2_AbleCancel());
        }

        private void AbleStop_Click(Object eventSender, EventArgs eventArgs)
        {
            FuncQuit("LKIF2_AbleStop", LKIF2.LKIF2_AbleStop());
        }

        private void SetReflectionMode_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_REFLECTIONMODE reflectionMode = (LKIF2.LKIF_REFLECTIONMODE)0;
            LKIF2.RC result = (LKIF2.RC)0;
            int tempRefParam = (int)reflectionMode;
            if (GetComboData(SetReflectionModeParam[0], ref headNo) && GetComboData(SetReflectionModeParam[1], ref tempRefParam))
            {
                reflectionMode = (LKIF2.LKIF_REFLECTIONMODE)tempRefParam;
                result = LKIF2.LKIF2_SetReflectionMode(headNo, reflectionMode);
            }
            else
            {
                reflectionMode = (LKIF2.LKIF_REFLECTIONMODE)tempRefParam;
                return;
            }

            FuncQuit("LKIF2_SetReflectionMode", result);
        }

        private void SetMask_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            int onOff = 0;
            int pos1 = 0;
            int pos2 = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(SetMaskParam1[0], ref headNo) && GetComboData(SetMaskParam1[1], ref onOff) &&
                GetTextData(SetMaskParam2[0], ref pos1) && GetTextData(SetMaskParam2[1], ref pos2))
            {
                result = LKIF2.LKIF2_SetMask(headNo, onOff, pos1, pos2);
            }
            else
            {
                return;
            }
            FuncQuit("LKIF2_SetMask", result);
        }

        private void SetMedian_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_MEDIAN median = (LKIF2.LKIF_MEDIAN)0;
            LKIF2.RC result = (LKIF2.RC)0;
            int tempRefParam = (int)median;
            if (GetComboData(SetMedianParam[0], ref headNo) && GetComboData(SetMedianParam[1], ref tempRefParam))
            {
                median = (LKIF2.LKIF_MEDIAN)tempRefParam;
                result = LKIF2.LKIF2_SetMedian(headNo, median);
            }
            else
            {
                median = (LKIF2.LKIF_MEDIAN)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetMedian", result);
        }

        private void SetLaserCtrlGroup_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_LASER_CTRL_GROUP laserCtrlGroup = (LKIF2.LKIF_LASER_CTRL_GROUP)0;
            LKIF2.RC result = (LKIF2.RC)0;
            int tempRefParam = (int)laserCtrlGroup;
            if (GetComboData(SetLaserCtrlGroupParam[0], ref headNo) && GetComboData(SetLaserCtrlGroupParam[1], ref tempRefParam))
            {
                laserCtrlGroup = (LKIF2.LKIF_LASER_CTRL_GROUP)tempRefParam;
                result = LKIF2.LKIF2_SetLaserCtrlGroup(headNo, laserCtrlGroup);
            }
            else
            {
                laserCtrlGroup = (LKIF2.LKIF_LASER_CTRL_GROUP)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetLaserCtrlGroup", result);
        }

        private void SetRange_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_RANGE range = (LKIF2.LKIF_RANGE)0;
            LKIF2.RC result = (LKIF2.RC)0;
            int tempRefParam = (int)range;
            if (GetComboData(SetRangeParam[0], ref headNo) && GetComboData(SetRangeParam[1], ref tempRefParam))
            {
                range = (LKIF2.LKIF_RANGE)tempRefParam;
                result = LKIF2.LKIF2_SetRange(headNo, range);
            }
            else
            {
                range = (LKIF2.LKIF_RANGE)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetRange", result);
        }

        private void SetMutualInterferencePreventionGroup_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP group = (LKIF2.LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP)0;
            LKIF2.RC result = (LKIF2.RC)0;
            int tempRefParam = (int)group;
            if (GetComboData(SetMutualInterferencePreventionGroupParam[0], ref headNo) && GetComboData(SetMutualInterferencePreventionGroupParam[1], ref tempRefParam))
            {
                group = (LKIF2.LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP)tempRefParam;
                result = LKIF2.LKIF2_SetMutualInterferencePreventionGroup(headNo, group);
            }
            else
            {
                group = (LKIF2.LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetMutualInterferencePreventionGroup", result);
        }

        private void SetCalcMethod_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            int head_Out_No = 0;
            LKIF2.LKIF_CALCMETHOD method = (LKIF2.LKIF_CALCMETHOD)0;
            LKIF2.RC result = (LKIF2.RC)0;

            int tempRefParam = (int)method;
            if (GetComboData(SetCalcMethodParam1[0], ref outNo) && GetComboData(SetCalcMethodParam1[1], ref tempRefParam) && GetTextData(SetCalcMethodParam2, ref head_Out_No))
            {
                method = (LKIF2.LKIF_CALCMETHOD)tempRefParam;
                result = LKIF2.LKIF2_SetCalcMethod(outNo, method, head_Out_No);
            }
            else
            {
                method = (LKIF2.LKIF_CALCMETHOD)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetCalcMethod", result);
        }

        private void SetCalcTarget_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.LKIF_CALCTARGET target = (LKIF2.LKIF_CALCTARGET)0;
            LKIF2.RC result = (LKIF2.RC)0;
            int tempRefParam = (int)target;
            if (GetComboData(SetCalcTargetParam[0], ref outNo) && GetComboData(SetCalcTargetParam[1], ref tempRefParam))
            {
                target = (LKIF2.LKIF_CALCTARGET)tempRefParam;
                result = LKIF2.LKIF2_SetCalcTarget(outNo, target);
            }
            else
            {
                target = (LKIF2.LKIF_CALCTARGET)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetCalcTarget", result);
        }

        private void SetOutAddSub_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            int addSub1 = 0;
            int addSub2 = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(SetOutAddSubParam[0], ref outNo) && GetComboData(SetOutAddSubParam[1], ref addSub1) && GetComboData(SetOutAddSubParam[2], ref addSub2))
            {
                result = LKIF2.LKIF2_SetOutAddSub(outNo, addSub1, addSub2);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_SetOutAddSub", result);
        }

        private void SetOutOperation_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            LKIF2.LKIF_OUTNO targetOut = MakeOutNoFromCheckBoxes(SetOutOperationParam2);
            if (GetComboData(SetOutOperationParam1, ref outNo))
            {
                result = LKIF2.LKIF2_SetOutOperation(outNo, targetOut);
            }
            else
            {
                return;
            }
            FuncQuit("LKIF2_SetOutOperation", result);
        }

        private void SetScaling_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            int inputValue1 = 0;
            int outputValue1 = 0;
            int inputValue2 = 0;
            int outputValue2 = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(SetScalingParam1, ref outNo) && GetTextData(SetScalingParam2[0], ref inputValue1) && GetTextData(SetScalingParam2[1], ref outputValue1) && 
                GetTextData(SetScalingParam2[2], ref inputValue2) && GetTextData(SetScalingParam2[3], ref outputValue2))
            {
                result = LKIF2.LKIF2_SetScaling(outNo, inputValue1, outputValue1, inputValue2, outputValue2);
            }
            else
            {
                return;
            }
            FuncQuit("LKIF2_SetScaling", result);
        }

        private void SetFilter_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            LKIF2.LKIF_FILTERPARA filterPara = (LKIF2.LKIF_FILTERPARA)0;
            LKIF2.LKIF_FILTERMODE filterMode = (LKIF2.LKIF_FILTERMODE)0;
            int tempRefParam = (int)filterMode;
            int tempRefParam2 = (int)filterPara;
            if (GetComboData(SetFilterParam[0], ref outNo) && GetComboData(SetFilterParam[1], ref tempRefParam) && GetComboData(SetFilterParam[2], ref tempRefParam2))
            {
                filterPara = (LKIF2.LKIF_FILTERPARA)tempRefParam2;
                filterMode = (LKIF2.LKIF_FILTERMODE)tempRefParam;
                result = LKIF2.LKIF2_SetFilter(outNo, filterMode, filterPara);
            }
            else
            {
                filterPara = (LKIF2.LKIF_FILTERPARA)tempRefParam2;
                filterMode = (LKIF2.LKIF_FILTERMODE)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetFilter", result);
        }

        private void SetTriggerMode_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            LKIF2.LKIF_TRIGGERMODE triggerMode = (LKIF2.LKIF_TRIGGERMODE)0;
            int tempRefParam = (int)triggerMode;
            if (GetComboData(SetTriggerModeParam[0], ref outNo) && GetComboData(SetTriggerModeParam[1], ref tempRefParam))
            {
                triggerMode = (LKIF2.LKIF_TRIGGERMODE)tempRefParam;
                result = LKIF2.LKIF2_SetTriggerMode(outNo, triggerMode);
            }
            else
            {
                triggerMode = (LKIF2.LKIF_TRIGGERMODE)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetTriggerMode", result);
        }

        private void SetOffset_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            int offset = 0;
            if (GetComboData(SetOffsetParam1, ref outNo) && GetTextData(SetOffsetParam2, ref offset))
            {
                result = LKIF2.LKIF2_SetOffset(outNo, offset);
            }
            else
            {
                return;
            }
            FuncQuit("LKIF2_SetOffset", result);
        }

        private void SetCalcMode_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            LKIF2.LKIF_CALCMODE CalcMode = (LKIF2.LKIF_CALCMODE)0;
            int tempRefParam = (int)CalcMode;
            if (GetComboData(SetCalcModeParam[0], ref outNo) && GetComboData(SetCalcModeParam[1], ref tempRefParam))
            {
                CalcMode = (LKIF2.LKIF_CALCMODE)tempRefParam;
                result = LKIF2.LKIF2_SetCalcMode(outNo, CalcMode);
            }
            else
            {
                CalcMode = (LKIF2.LKIF_CALCMODE)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetCalcMode", result);
        }

        private void SetAnalogScaling_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            int inputValue1 = 0;
            int outputVoltage1 = 0;
            int inputValue2 = 0;
            int outputVoltage2 = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(SetAnalogScalingParam1, ref outNo) && GetTextData(SetAnalogScalingParam2[0], ref inputValue1) && 
                GetTextData(SetAnalogScalingParam2[1], ref outputVoltage1) && GetTextData(SetAnalogScalingParam2[2], ref inputValue2) && 
                GetTextData(SetAnalogScalingParam2[3], ref outputVoltage2))
            {
                result = LKIF2.LKIF2_SetAnalogScaling(outNo, inputValue1, outputVoltage1, inputValue2, outputVoltage2);
            }
            else
            {
                return;
            }

            FuncQuit("LKIF2_SetAnalogScaling", result);
        }

        private void SetDisplayUnit_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            LKIF2.LKIF_DISPLAYUNIT displayUnit = (LKIF2.LKIF_DISPLAYUNIT)0;
            int tempRefParam = (int)displayUnit;
            if (GetComboData(SetDisplayUnitParam[0], ref outNo) && GetComboData(SetDisplayUnitParam[1], ref tempRefParam))
            {
                displayUnit = (LKIF2.LKIF_DISPLAYUNIT)tempRefParam;
                result = LKIF2.LKIF2_SetDisplayUnit(outNo, displayUnit);
            }
            else
            {
                displayUnit = (LKIF2.LKIF_DISPLAYUNIT)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetDisplayUnit", result);
        }

        private void SetMeasureType_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            LKIF2.LKIF_MEASURETYPE measureType = (LKIF2.LKIF_MEASURETYPE)0;
            int tempRefParam = (int)measureType;
            if (GetComboData(SetMeasureTypeParam[0], ref outNo) && GetComboData(SetMeasureTypeParam[1], ref tempRefParam))
            {
                measureType = (LKIF2.LKIF_MEASURETYPE)tempRefParam;
                result = LKIF2.LKIF2_SetMeasureType(outNo, measureType);
            }
            else
            {
                measureType = (LKIF2.LKIF_MEASURETYPE)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetMeasureType", result);
        }

        private void SetSynchronization_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            int onOff = 0;
            if (GetComboData(SetSynchronizationParam[0], ref outNo) && GetComboData(SetSynchronizationParam[1], ref onOff))
            {
                result = LKIF2.LKIF2_SetSynchronization(outNo, onOff);
            }
            else
            {
                return;
            }
            FuncQuit("LKIF2_SetSynchronization", result);
        }

        private void SetStorageTarget_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            int onOff = 0;
            if (GetComboData(SetStorageTargetParam[0], ref outNo) && GetComboData(SetStorageTargetParam[1], ref onOff))
            {
                result = LKIF2.LKIF2_SetStorageTarget(outNo, onOff);
            }
            else
            {
                return;
            }
            FuncQuit("LKIF2_SetStorageTarget", result);
        }

        private void SetSamplingCycle_Click(Object eventSender, EventArgs eventArgs)
        {
            LKIF2.RC result = (LKIF2.RC)0;
            LKIF2.LKIF_SAMPLINGCYCLE samplingCycle = (LKIF2.LKIF_SAMPLINGCYCLE)0;
            int tempRefParam = (int)samplingCycle;
            if (GetComboData(SetSamplingCycleParam, ref tempRefParam))
            {
                samplingCycle = (LKIF2.LKIF_SAMPLINGCYCLE)tempRefParam;
                result = LKIF2.LKIF2_SetSamplingCycle(samplingCycle);
            }
            else
            {
                samplingCycle = (LKIF2.LKIF_SAMPLINGCYCLE)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetSamplingCycle", result);
        }

        private void SetMutualInterferencePrevention_Click(Object eventSender, EventArgs eventArgs)
        {
            LKIF2.RC result = (LKIF2.RC)0;
            LKIF2.LKIF_MUTUAL_INTERFERENCE_PREVENTION onOff = (LKIF2.LKIF_MUTUAL_INTERFERENCE_PREVENTION)0;
            int tempRefParam = (int)onOff;
            if (GetComboData(SetMutualInterferencePreventionParam, ref tempRefParam))
            {
                onOff = (LKIF2.LKIF_MUTUAL_INTERFERENCE_PREVENTION)tempRefParam;
                result = LKIF2.LKIF2_SetMutualInterferencePrevention(onOff);
            }
            else
            {
                onOff = (LKIF2.LKIF_MUTUAL_INTERFERENCE_PREVENTION)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetMutualInterferencePrevention", result);
        }

        private void SetToleranceComparatorOutputFormat_Click(Object eventSender, EventArgs eventArgs)
        {
            LKIF2.RC result = (LKIF2.RC)0;
            LKIF2.LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT toleranceComparatorOutputFormat = (LKIF2.LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT)0;
            int tempRefParam = (int)toleranceComparatorOutputFormat;
            if (GetComboData(SetToleranceComparatorOutputFormatParam, ref tempRefParam))
            {
                toleranceComparatorOutputFormat = (LKIF2.LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT)tempRefParam;
                result = LKIF2.LKIF2_SetToleranceComparatorOutputFormat(toleranceComparatorOutputFormat);
            }
            else
            {
                toleranceComparatorOutputFormat = (LKIF2.LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetToleranceComparatorOutputFormat", result);
        }

        private void SetStorobeTime_Click(Object eventSender, EventArgs eventArgs)
        {
            LKIF2.RC result = (LKIF2.RC)0;
            LKIF2.LKIF_STOROBETIME storobeTime = (LKIF2.LKIF_STOROBETIME)0;
            int tempRefParam = (int)storobeTime;
            if (GetComboData(SetStorobeTimeParam, ref tempRefParam))
            {
                storobeTime = (LKIF2.LKIF_STOROBETIME)tempRefParam;
                result = LKIF2.LKIF2_SetStrobeTime(storobeTime);
            }
            else
            {
                storobeTime = (LKIF2.LKIF_STOROBETIME)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetStrobeTime", result);
        }

        private void SetDataStorage_Click(Object eventSender, EventArgs eventArgs)
        {
            LKIF2.RC result = (LKIF2.RC)0;
            int storageNum = 0;
            LKIF2.LKIF_STORAGECYCLE storageCycle = (LKIF2.LKIF_STORAGECYCLE)0;
            int tempRefParam = (int)storageCycle;
            if (GetTextData(SetDataStorageParam1, ref storageNum) && GetComboData(SetDataStorageParam2, ref tempRefParam))
            {
                storageCycle = (LKIF2.LKIF_STORAGECYCLE)tempRefParam;
                result = LKIF2.LKIF2_SetDataStorage(storageNum, storageCycle);
            }
            else
            {
                storageCycle = (LKIF2.LKIF_STORAGECYCLE)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetDataStorage", result);
        }

        private void SetAnalogChannel_Click(Object eventSender, EventArgs eventArgs)
        {
            int chNo = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            int outNo = 0;
            if (GetComboData(SetAnalogChannelParam[0], ref chNo) && GetComboData(SetAnalogChannelParam[1], ref outNo))
            {
                result = LKIF2.LKIF2_SetAnalogChannel(chNo, outNo);
            }
            else
            {
                return;
            }
            FuncQuit("LKIF2_SetAnalogChannel", result);
        }

        private void SetAlarmOutputForm_Click(Object eventSender, EventArgs eventArgs)
        {
            LKIF2.LKIF_ALARM_OUTPUT_FORM alarmOutputForm = (LKIF2.LKIF_ALARM_OUTPUT_FORM)0;
            LKIF2.RC result = (LKIF2.RC)0;
            int tempRefParam = (int)alarmOutputForm;
            if (GetComboData(SetAlarmOutputFormParam, ref tempRefParam))
            {
                alarmOutputForm = (LKIF2.LKIF_ALARM_OUTPUT_FORM)tempRefParam;
                result = LKIF2.LKIF2_SetAlarmOutputForm(alarmOutputForm);
            }
            else
            {
                alarmOutputForm = (LKIF2.LKIF_ALARM_OUTPUT_FORM)tempRefParam;
                return;
            }
            FuncQuit("LKIF2_SetAlarmOutputForm", result);
        }

        private void SetNumOfUsedHeads_Click(Object eventSender, EventArgs eventArgs)
        {
            int numOfUsedHeads = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetTextData(SetNumOfUsedHeadsParam, ref numOfUsedHeads))
            {
                result = LKIF2.LKIF2_SetNumOfUsedHeads(numOfUsedHeads);
            }
            else
            {
                return;
            }
            FuncQuit("LKIF2_SetNumOfUsedHeads", result);
        }

        private void SetNumOfUsedOut_Click(Object eventSender, EventArgs eventArgs)
        {
            int numOfUsedOut = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetTextData(SetNumOfUsedOutParam, ref numOfUsedOut))
            {
                result = LKIF2.LKIF2_SetNumOfUsedOut(numOfUsedOut);
            }
            else
            {
                return;
            }
            FuncQuit("LKIF2_SetNumOfUsedOut", result);
        }

        private void SetNumOfUsedAnalogCh_Click(Object eventSender, EventArgs eventArgs)
        {
            int numOfUsedAnalogCh = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetTextData(SetNumOfUsedAnalogChParam, ref numOfUsedAnalogCh))
            {
                result = LKIF2.LKIF2_SetNumOfUsedAnalogCh(numOfUsedAnalogCh);
            }
            else
            {
                return;
            }
            FuncQuit("LKIF2_SetNumOfUsedAnalogCh", result);
        }

        private void GetPanel_Click(Object eventSender, EventArgs eventArgs)
        {
            int upperDisp = 0;
            int lowerDisp = 0;

            LKIF2.RC result = LKIF2.LKIF2_GetPanel(ref upperDisp, ref lowerDisp);

            if (IsSuccess(result))
            {
                upperDisp++;
                lowerDisp++;
                GetPanelParam[0].Text = GetListText(SetPanelParam[0],upperDisp);
                GetPanelParam[1].Text = GetListText(SetPanelParam[1],lowerDisp);
            }
            FuncQuit("LKIF2_GetPanel", result);
        }

        private void GetTolerance_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            int upperLimit = 0;
            int lowerLimit = 0;
            int hysteresis = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(GetToleranceParam1, ref outNo))
            {
                result = LKIF2.LKIF2_GetTolerance(outNo, ref upperLimit, ref lowerLimit, ref hysteresis);
            }
            else
            {
                return;
            }
            if (IsSuccess(result))
            {
                GetToleranceParam2[0].Text = upperLimit.ToString();
                GetToleranceParam2[1].Text = lowerLimit.ToString();
                GetToleranceParam2[2].Text = hysteresis.ToString();
            }

            FuncQuit("LKIF2_GetTolerance", result);
        }

        private void GetAbleMode_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_ABLEMODE ableMode = (LKIF2.LKIF_ABLEMODE)0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(GetAbleModeParam1, ref headNo))
            {
                result = LKIF2.LKIF2_GetAbleMode(headNo, ref ableMode);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetAbleModeParam2.Text = GetListText(SetAbleModeParam[1],(int)ableMode);
            }

            FuncQuit("LKIF2_GetAbleMode", result);
        }

        private void GetAbleMinMax_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            int min = 0;
            int max = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(GetAbleMinMaxParam1, ref headNo))
            {
                result = LKIF2.LKIF2_GetAbleMinMax(headNo, ref min, ref max);
            }
            else
            {
                return;
            }
            if (IsSuccess(result))
            {
                GetAbleMinMaxParam2.Text = min.ToString();
                GetAbleMinMaxParam3.Text = max.ToString();
            }

            FuncQuit("LKIF2_GetAbleMinMax", result);
        }

        private void GetMeasureMode_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_MEASUREMODE measureMode = (LKIF2.LKIF_MEASUREMODE)0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetMeasureModeParam1, ref headNo))
            {
                result = LKIF2.LKIF2_GetMeasureMode(headNo, ref measureMode);
            }
            else
            {
                return;
            }
            if (IsSuccess(result))
            {
                GetMeasureModeParam2.Text = GetListText(SetMeasureModeParam[1],(int)measureMode);
            }

            FuncQuit("LKIF2_GetMeasureMode", result);
        }

        private void GetBasicPoint_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_BASICPOINT basicPoint = (LKIF2.LKIF_BASICPOINT)0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(GetBasicPointParam1, ref headNo))
            {
                result = LKIF2.LKIF2_GetBasicPoint(headNo, ref basicPoint);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetBasicPointParam2.Text = GetListText(SetBasicPointParam[1],(int)basicPoint);
            }

            FuncQuit("LKIF2_GetBasicPoint", result);
        }

        private void GetNumAlarm_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            int numAlarm = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(GetNumAlarmParam1, ref headNo))
            {
                result = LKIF2.LKIF2_GetNumAlarm(headNo, ref numAlarm);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetNumAlarmParam2.Text = numAlarm.ToString();
            }

            FuncQuit("LKIF2_GetNumAlarm", result);
        }

        private void GetNumRecovery_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            int recoveryNum = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(GetNumRecoveryParam1, ref headNo))
            {
                result = LKIF2.LKIF2_GetNumRecovery(headNo, ref recoveryNum);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetNumRecoveryParam2.Text = recoveryNum.ToString();
            }

            FuncQuit("LKIF2_GetNumRecovery", result);
        }

        private void GetAlarmLevel_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            int alaramLevel = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(GetAlarmLevelParam1, ref headNo))
            {
                result = LKIF2.LKIF2_GetAlarmLevel(headNo, ref alaramLevel);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetAlarmLevelParam2.Text = alaramLevel.ToString();
            }

            FuncQuit("LKIF2_GetAlarmLevel", result);
        }

        private void GetReflectionMode_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_REFLECTIONMODE reflectionMode = (LKIF2.LKIF_REFLECTIONMODE)0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(GetReflectionModeParam1, ref headNo))
            {
                result = LKIF2.LKIF2_GetReflectionMode(headNo, ref reflectionMode);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetReflectionModeParam2.Text = GetListText(SetReflectionModeParam[1],(int)reflectionMode);
            }

            FuncQuit("LKIF2_GetReflectionMode", result);
        }

        private void GetMask_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            int onOff = 0;
            int pos1 = 0;
            int pos2 = 0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(GetMaskParam1, ref headNo))
            {
                result = LKIF2.LKIF2_GetMask(headNo, ref onOff, ref pos1, ref pos2);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                onOff = Convert.ToInt32(-(onOff - 1));
                GetMaskParam2[0].Text = GetListText(SetMaskParam1[1],onOff);
                GetMaskParam2[1].Text = pos1.ToString();
                GetMaskParam2[2].Text = pos2.ToString();
            }

            FuncQuit("LKIF2_GetMask", result);
        }

        private void GetMedian_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_MEDIAN median = (LKIF2.LKIF_MEDIAN)0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(GetMedianParam1, ref headNo))
            {
                result = LKIF2.LKIF2_GetMedian(headNo, ref median);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetMedianParam2.Text = GetListText(SetMedianParam[1],(int)median);
            }

            FuncQuit("LKIF2_GetMedian", result);
        }

        private void GetLaserCtrlGroup_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_LASER_CTRL_GROUP laserCtrlGroup = (LKIF2.LKIF_LASER_CTRL_GROUP)0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(GetLaserCtrlGroupParam1, ref headNo))
            {
                result = LKIF2.LKIF2_GetLaserCtrlGroup(headNo, ref laserCtrlGroup);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetLaserCtrlGroupParam2.Text = GetListText(SetLaserCtrlGroupParam[1],(int)laserCtrlGroup);
            }

            FuncQuit("LKIF2_GetLaserCtrlGroup", result);
        }

        private void GetRange_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_RANGE range = (LKIF2.LKIF_RANGE)0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetRangeParam1, ref headNo))
            {
                result = LKIF2.LKIF2_GetRange(headNo, ref range);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetRangeParam2.Text = GetListText(SetRangeParam[1],(int)range);
            }

            FuncQuit("LKIF2_GetRange", result);
        }

        private void GetMutualInterferencePreventionGroup_Click(Object eventSender, EventArgs eventArgs)
        {
            int headNo = 0;
            LKIF2.LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP group = (LKIF2.LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP)0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetMutualInterferencePreventionGroupParam1, ref headNo))
            {
                result = LKIF2.LKIF2_GetMutualInterferencePreventionGroup(headNo, ref group);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetMutualInterferencePreventionGroupParam2.Text = GetListText(SetMutualInterferencePreventionGroupParam[1],(int)group);
            }

            FuncQuit("LKIF2_GetMutualInterferencePreventionGroup", result);
        }

        private void GetCalcMethod_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.LKIF_CALCMETHOD calcMethod = (LKIF2.LKIF_CALCMETHOD)0;
            int head_Out_No = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetCalcMethodParam1, ref outNo))
            {
                result = LKIF2.LKIF2_GetCalcMethod(outNo, ref calcMethod, ref head_Out_No);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetCalcMethodParam2[0].Text = GetListText(SetCalcMethodParam1[1],(int)calcMethod);
                GetCalcMethodParam2[1].Text = head_Out_No.ToString();
            }

            FuncQuit("LKIF2_GetCalcMethod", result);
        }

        private void GetCalcTarget_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.LKIF_CALCTARGET calcTarget = (LKIF2.LKIF_CALCTARGET)0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetCalcTargetParam1, ref outNo))
            {
                result = LKIF2.LKIF2_GetCalcTarget(outNo, ref calcTarget);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetCalcTargetParam2.Text = GetListText(SetCalcTargetParam[1],(int)calcTarget);
            }

            FuncQuit("LKIF2_GetCalcTarget", result);
        }

        private void GetOutAddSub_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            int addSub1 = 0;
            int addSub2 = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetOutAddSubParam1, ref outNo))
            {
                result = LKIF2.LKIF2_GetOutAddSub(outNo, ref addSub1, ref addSub2);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetOutAddSubParam2[0].Text = GetListText(SetOutAddSubParam[1],addSub1);
                GetOutAddSubParam2[1].Text = GetListText(SetOutAddSubParam[2],addSub2);
            }

            FuncQuit("LKIF2_GetOutAddSub", result);
        }

        private void GetOutOperation_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.LKIF_OUTNO targetOut = (LKIF2.LKIF_OUTNO)0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetOutOperationParam1, ref outNo))
            {
                result = LKIF2.LKIF2_GetOutOperation(outNo, ref targetOut);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                SetCheckBoxesForOutNo(GetOutOperationParam2, targetOut);
            }

            FuncQuit("LKIF2_GetOutOperation", result);
        }

        private void GetScaling_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            int inputValue1 = 0;
            int outputValue1 = 0;
            int inputValue2 = 0;
            int outputValue2 = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetScalingParam1, ref outNo))
            {
                result = LKIF2.LKIF2_GetScaling(outNo, ref inputValue1, ref outputValue1, ref inputValue2, ref outputValue2);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetScalingParam2[0].Text = inputValue1.ToString();
                GetScalingParam2[1].Text = outputValue1.ToString();
                GetScalingParam2[2].Text = inputValue2.ToString();
                GetScalingParam2[3].Text = outputValue2.ToString();
            }

            FuncQuit("LKIF2_GetScaling", result);
        }

        private void GetFilter_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.LKIF_FILTERMODE filterMode = (LKIF2.LKIF_FILTERMODE)0;
            LKIF2.LKIF_FILTERPARA filterPara = (LKIF2.LKIF_FILTERPARA)0;
            LKIF2.RC result = (LKIF2.RC)0;
            if (GetComboData(GetFilterParam1, ref outNo))
            {
                result = LKIF2.LKIF2_GetFilter(outNo, ref filterMode, ref filterPara);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetFilterParam2[0].Text = GetListText(SetFilterParam[1],(int)filterMode);
                GetFilterParam2[1].Text = GetFileterParamString(filterMode, filterPara);
            }

            FuncQuit("LKIF2_GetFilter", result);
        }

        private void GetTriggerMode_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.LKIF_TRIGGERMODE triggerMode = (LKIF2.LKIF_TRIGGERMODE)0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetTriggerModeParam1, ref outNo))
            {
                result = LKIF2.LKIF2_GetTriggerMode(outNo, ref triggerMode);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetTriggerModeParam2.Text = GetListText(SetTriggerModeParam[1],(int)triggerMode);
            }

            FuncQuit("LKIF2_GetTriggerMode", result);
        }

        private void GetOffset_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            int offset = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetOffsetParam1, ref outNo))
            {
                result = LKIF2.LKIF2_GetOffset(outNo, ref offset);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetOffsetParam2.Text = offset.ToString();
            }

            FuncQuit("LKIF2_GetOffset", result);
        }

        private void GetCalcMode_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.LKIF_CALCMODE CalcMode = (LKIF2.LKIF_CALCMODE)0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetCalcModeParam1, ref outNo))
            {
                result = LKIF2.LKIF2_GetCalcMode(outNo, ref CalcMode);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetCalcModeParam2.Text = GetListText(SetCalcModeParam[1],(int)CalcMode);
            }

            FuncQuit("LKIF2_GetCalcMode", result);
        }

        private void GetAnalogScaling_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            int inputValue1 = 0;
            int outputVoltage1 = 0;
            int inputValue2 = 0;
            int outputVoltage2 = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetAnalogScalingParam1, ref outNo))
            {
                result = LKIF2.LKIF2_GetAnalogScaling(outNo, ref inputValue1, ref outputVoltage1, ref inputValue2, ref outputVoltage2);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetAnalogScalingParam2[0].Text = inputValue1.ToString();
                GetAnalogScalingParam2[1].Text = outputVoltage1.ToString();
                GetAnalogScalingParam2[2].Text = inputValue2.ToString();
                GetAnalogScalingParam2[3].Text = outputVoltage2.ToString();
            }

            FuncQuit("LKIF2_GetAnalogScaling", result);
        }

        private void GetDisplayUnit_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.LKIF_MEASURETYPE measureType = (LKIF2.LKIF_MEASURETYPE)0;
            LKIF2.LKIF_DISPLAYUNIT displayUnit = (LKIF2.LKIF_DISPLAYUNIT)0;
            LKIF2.RC[] result = new LKIF2.RC[3];

            if (GetComboData(GetDisplayUnitParam1, ref outNo))
            {
                result[0] = LKIF2.LKIF2_GetMeasureType(outNo, ref measureType);
                result[1] = LKIF2.LKIF2_GetDisplayUnit(outNo, ref displayUnit);
            }
            else
            {
                return;
            }

            if (IsSuccess(result[0]) && IsSuccess(result[1]))
            {
                GetDisplayUnitParam2.Text = GetDisplayUnitParamString(measureType, displayUnit);
            }

            FuncQuit("LKIF2_GetDisplayUnit", result[1]);
        }

        private void GetMeasureType_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            LKIF2.LKIF_MEASURETYPE measureType = (LKIF2.LKIF_MEASURETYPE)0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetMeasureTypeParam1, ref outNo))
            {
                result = LKIF2.LKIF2_GetMeasureType(outNo, ref measureType);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetMeasureTypeParam2.Text = GetListText(SetMeasureTypeParam[1],(int)measureType);
            }

            FuncQuit("LKIF2_GetMeasureType", result);
        }

        private void GetSynchronization_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            int onOff = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetSynchronizationParam1, ref outNo))
            {
                result = LKIF2.LKIF2_GetSynchronization(outNo, ref onOff);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                onOff = Convert.ToInt32(-(onOff - 1));
                GetSynchronizationParam2.Text = GetListText(SetSynchronizationParam[1],onOff);
            }

            FuncQuit("LKIF2_GetSynchronization", result);
        }

        private void GetStorageTarget_Click(Object eventSender, EventArgs eventArgs)
        {
            int outNo = 0;
            int onOff = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetStorageTargetParam1, ref outNo))
            {
                result = LKIF2.LKIF2_GetStorageTarget(outNo, ref onOff);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                onOff = Convert.ToInt32(-(onOff - 1));
                GetStorageTargetParam2.Text = GetListText(SetStorageTargetParam[1],onOff);
            }

            FuncQuit("LKIF2_GetStorageTarget", result);
        }

        private void GetSamplingCycle_Click(Object eventSender, EventArgs eventArgs)
        {
            LKIF2.LKIF_SAMPLINGCYCLE samplingCycle = (LKIF2.LKIF_SAMPLINGCYCLE)0;

            LKIF2.RC result = LKIF2.LKIF2_GetSamplingCycle(ref samplingCycle);

            if (IsSuccess(result))
            {
                GetSamplingCycleParam.Text = GetListText(SetSamplingCycleParam,(int)samplingCycle);
            }

            FuncQuit("LKIF2_GetSamplingCycle", result);
        }

        private void GetMutualInterferencePrevention_Click(Object eventSender, EventArgs eventArgs)
        {
            LKIF2.LKIF_MUTUAL_INTERFERENCE_PREVENTION onOff = (LKIF2.LKIF_MUTUAL_INTERFERENCE_PREVENTION)0;

            LKIF2.RC result = LKIF2.LKIF2_GetMutualInterferencePrevention(ref onOff);

            if (IsSuccess(result))
            {
                GetMutualInterferencePreventionParam.Text = GetListText(SetMutualInterferencePreventionParam,(int)onOff);
            }

            FuncQuit("LKIF2_GetMutualInterferencePrevention", result);
        }

        private void GetToleranceComparatorOutputFormat_Click(Object eventSender, EventArgs eventArgs)
        {
            LKIF2.LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT toleranceComparatorOutputFormat = (LKIF2.LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT)0;

            LKIF2.RC result = LKIF2.LKIF2_GetToleranceComparatorOutputFormat(ref toleranceComparatorOutputFormat);

            if (IsSuccess(result))
            {
                GetToleranceComparatorOutputFormatParam.Text = GetListText(SetToleranceComparatorOutputFormatParam,(int)toleranceComparatorOutputFormat);
            }

            FuncQuit("LKIF2_GetToleranceComparatorOutputFormat", result);
        }

        private void GetStorobeTime_Click(Object eventSender, EventArgs eventArgs)
        {
            LKIF2.LKIF_STOROBETIME storobeTime = (LKIF2.LKIF_STOROBETIME)0;

            LKIF2.RC result = LKIF2.LKIF2_GetStrobeTime(ref storobeTime);

            if (IsSuccess(result))
            {
                GetStorobeTimeParam.Text =GetListText( SetStorobeTimeParam,(int)storobeTime);
            }

            FuncQuit("LKIF2_GetStrobeTime", result);
        }

        private void GetDataStorage_Click(Object eventSender, EventArgs eventArgs)
        {
            int storageNum = 0;
            LKIF2.LKIF_STORAGECYCLE storageCycle = (LKIF2.LKIF_STORAGECYCLE)0;
            LKIF2.RC result = LKIF2.LKIF2_GetDataStorage(ref storageNum, ref storageCycle);

            if (IsSuccess(result))
            {
                GetDataStorageParam[0].Text = storageNum.ToString();
                GetDataStorageParam[1].Text = GetListText(SetDataStorageParam2,(int)storageCycle);
            }

            FuncQuit("LKIF2_GetDataStorage", result);
        }

        private void GetAnalogChannel_Click(Object eventSender, EventArgs eventArgs)
        {
            int chNo = 0;
            int outNo = 0;
            LKIF2.RC result = (LKIF2.RC)0;

            if (GetComboData(GetAnalogChannelParam1, ref chNo))
            {
                result = LKIF2.LKIF2_GetAnalogChannel(chNo, ref outNo);
            }
            else
            {
                return;
            }

            if (IsSuccess(result))
            {
                GetAnalogChannelParam2.Text = GetListText(SetAnalogChannelParam[1],outNo);
            }

            FuncQuit("LKIF2_GetAnalogChannel", result);
        }

        private void GetAlarmOutputForm_Click(Object eventSender, EventArgs eventArgs)
        {
            LKIF2.LKIF_ALARM_OUTPUT_FORM alarmOutputForm = (LKIF2.LKIF_ALARM_OUTPUT_FORM)0;

            LKIF2.RC result = LKIF2.LKIF2_GetAlarmOutputForm(ref alarmOutputForm);

            if (IsSuccess(result))
            {
                GetAlarmOutputFormParam.Text = GetListText(SetAlarmOutputFormParam,(int)alarmOutputForm);
            }

            FuncQuit("LKIF2_GetAlarmOutputForm", result);
        }

        private void GetNumOfUsedHeads_Click(Object eventSender, EventArgs eventArgs)
        {
            int numOfUsedHeads = 0;

            LKIF2.RC result = LKIF2.LKIF2_GetNumOfUsedHeads(ref numOfUsedHeads);

            if (IsSuccess(result))
            {
                GetNumOfUsedHeadsParam.Text = numOfUsedHeads.ToString();
            }

            FuncQuit("LKIF2_GetNumOfUsedHeads", result);
        }

        private void GetNumOfUsedOut_Click(Object eventSender, EventArgs eventArgs)
        {
            int numOfUsedOut = 0;

            LKIF2.RC result = LKIF2.LKIF2_GetNumOfUsedOut(ref numOfUsedOut);

            if (IsSuccess(result))
            {
                GetNumOfUsedOutParam.Text = numOfUsedOut.ToString();
            }

            FuncQuit("LKIF2_GetNumOfUsedOut", result);
        }

        private void GetNumOfUsedAnalogCh_Click(Object eventSender, EventArgs eventArgs)
        {
            int numOfUsedAnalogCh = 0;

            LKIF2.RC result = LKIF2.LKIF2_GetNumOfUsedAnalogCh(ref numOfUsedAnalogCh);

            if (IsSuccess(result))
            {
                GetNumOfUsedAnalogChParam.Text = numOfUsedAnalogCh.ToString();
            }

            FuncQuit("LKIF2_GetNumOfUsedAnalogCh", result);
        }

        private void StartMeasure_Click(Object eventSender, EventArgs eventArgs)
        {
            FuncQuit("LKIF2_StartMeasure", LKIF2.LKIF2_StartMeasure());
        }

        private void StopMeasure_Click(Object eventSender, EventArgs eventArgs)
        {
            FuncQuit("LKIF2_StopMeasure", LKIF2.LKIF2_StopMeasure());
        }
        private void OpenDeviceUSB_Click(Object eventSender, EventArgs eventArgs)
        {
            FuncQuit("LKIF2_OpenDeviceUsb", LKIF2.LKIF2_OpenDeviceUsb());
        }

        private void OpenDeviceETHER_Click(Object eventSender, EventArgs eventArgs)
        {
            LKIF2.LKIF_OPENPARAM_ETHERNET openParam = new LKIF2.LKIF_OPENPARAM_ETHERNET();
            //example:"192.168.1.1"
            string ipAddres = OpenDeviceETHERParam1[0].Text + "." + OpenDeviceETHERParam1[1].Text + "." + OpenDeviceETHERParam1[2].Text + "." + OpenDeviceETHERParam1[3].Text;
            openParam.IPAddress.S_addr = inet_addr(ref ipAddres);

            FuncQuit("LKIF2_OpenDeviceETHER", LKIF2.LKIF2_OpenDeviceETHER(ref openParam));
        }

        private void CloseDevice_Click(Object eventSender, EventArgs eventArgs)
        {
            FuncQuit("LKIF2_CloseDevice", LKIF2.LKIF2_CloseDevice());
        }

        private void TabControl_SelectedIndexChanged(Object eventSender, EventArgs eventArgs)
        {
            //SSTab's bug fix
            foreach (Control ctl in Controls)
            {
                ComboBox cmb = ctl as ComboBox;
                if (cmb != null)
                {
                    cmb.SelectionLength = 0;
                }
            }
        }

        private void SetItemData(ComboBox comboBox, int[] itemData)
        {
            comboBox.DisplayMember = "Key";
            comboBox.ValueMember = "Value";
            System.Diagnostics.Debug.Assert(comboBox.Items.Count == itemData.Length);
            for (int i = 0; i < comboBox.Items.Count; i++)
            {
                KeyValuePair<string, int> item = new KeyValuePair<string,int>( comboBox.Items[i].ToString(), itemData[i]);
                comboBox.Items[i] = item;
            }
        }

        private string GetListText(ComboBox comboBox, int index)
        {
            KeyValuePair<string, int> item = (KeyValuePair<string, int>)comboBox.Items[index];
            return item.Key;
        }

        private void SetListItem(ComboBox comboBox, int index, string text)
        {
            KeyValuePair<string, int> item = (KeyValuePair<string, int>)comboBox.Items[index];
            KeyValuePair<string, int> newItem = new KeyValuePair<string,int>(text, item.Value);
            comboBox.Items[index] = newItem;
            comboBox.Update();
        }

        [STAThread]
        static void Main()
        {
            Application.Run(CreateInstance());
        }

        #endregion Methods
    }
}