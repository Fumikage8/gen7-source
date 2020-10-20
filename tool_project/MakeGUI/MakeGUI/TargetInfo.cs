namespace MakeGUI
{
    /// <summary>
    /// コンバート対象の情報
    /// </summary>
    struct TargetInfo
    {
        /// <summary>
        /// コメント
        /// </summary>
        public string Comment { get; set; }

        /// <summary>
        /// 付加情報
        /// </summary>
        public string Args { get; set; }


        /// <summary>
        /// 対応プラットホームビットフィールド
        /// </summary>
        public uint AvailablePlatform {get; set;}
    }

    struct PlatformParameter
    {
        public readonly uint Flag;
        public readonly string BatPath;
        public readonly string DisplayName;

        public PlatformParameter(string displayName, uint flag, string batPath)
        {
            this.DisplayName = displayName;
            this.Flag = flag;
            this.BatPath = batPath;
        }

    }

}
