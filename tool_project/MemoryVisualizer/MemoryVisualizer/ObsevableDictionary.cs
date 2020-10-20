using System.Collections.Specialized;
using System.ComponentModel;

namespace System.Collections.Generic
{
  public class ObservableDictionary<TKey, TValue>
      : CollectionBasedDictionary<TKey, TValue>, INotifyCollectionChanged, INotifyPropertyChanged
  {
    #region Constructor
    /// <summary>
    ///   空で、既定の初期量を備え、キーの型の既定の等値比較子を使用する、
    ///   <see cref="T:System.Collections.Generic.ObservableDictionary`2" /> クラスの
    ///   新しいインスタンスを初期化します。
    /// </summary>
    public ObservableDictionary()
      : base()
    {
    }

    /// <summary>
    ///   指定した <see cref="T:System.Collections.Generic.IDictionary`2" /> から要素をコピーして格納し、
    ///   キーの型の既定の等値比較子を使用する、
    ///   <see cref="T:System.Collections.Generic.ObservableDictionary`2" /> クラスの
    ///   新しいインスタンスを初期化します。
    /// </summary>
    /// <param name="dictionary">
    ///   新しい <see cref="T:System.Collections.Generic.ObservableDictionary`2" /> に要素がコピーされた 
    ///   <see cref="T:System.Collections.Generic.IDictionary`2" />。
    /// </param>
    /// <exception cref="T:System.ArgumentNullException">
    ///   <paramref name="dictionary" /> が null です。
    /// </exception>
    /// <exception cref="T:System.ArgumentException">
    ///   <paramref name="dictionary" /> に、1 つ以上の重複するキーが格納されています。
    /// </exception>
    public ObservableDictionary(IDictionary<TKey, TValue> dictionary)
      : base(dictionary)
    {
    }

    /// <summary>
    ///   空で、既定の初期量を備え、指定した <see cref="T:System.Collections.Generic.IEqualityComparer`1" /> を使用する、
    ///   <see cref="T:System.Collections.Generic.ObservableDictionary`2" /> クラスの
    ///   新しいインスタンスを初期化します。
    /// </summary>
    /// <param name="comparer">
    ///   キーの比較時に使用する <see cref="T:System.Collections.Generic.IEqualityComparer`1" /> 実装。
    ///   キーの型の既定の <see cref="T:System.Collections.Generic.EqualityComparer`1" /> を使用する場合は null。
    /// </param>
    public ObservableDictionary(IEqualityComparer<TKey> comparer)
      : base(comparer)
    {
    }

    /// <summary>
    ///   指定した <see cref="T:System.Collections.Generic.IDictionary`2" /> から要素をコピーして格納し、
    ///   指定した <see cref="T:System.Collections.Generic.IEqualityComparer`1" /> を使用する、
    ///   <see cref="T:System.Collections.Generic.ObservableDictionary`2" /> クラスの
    ///   新しいインスタンスを初期化します。
    /// </summary>
    /// <param name="dictionary">
    ///   新しい <see cref="T:System.Collections.Generic.ObservableDictionary`2" /> に要素がコピーされた
    ///   <see cref="T:System.Collections.Generic.IDictionary`2" />。
    /// </param>
    /// <param name="comparer">
    ///   キーの比較時に使用する <see cref="T:System.Collections.Generic.IEqualityComparer`1" /> 実装。
    ///   キーの型の既定の <see cref="T:System.Collections.Generic.EqualityComparer`1" /> を使用する場合は null。
    /// </param>
    /// <exception cref="T:System.ArgumentNullException">
    ///   <paramref name="dictionary" /> が null です。
    /// </exception>
    /// <exception cref="T:System.ArgumentException">
    ///   <paramref name="dictionary" /> に、1 つ以上の重複するキーが格納されています。
    /// </exception>
    public ObservableDictionary(IDictionary<TKey, TValue> dictionary, IEqualityComparer<TKey> comparer)
      : base(dictionary, comparer)
    {
    }
    #endregion // Constructor

    #region Field
    private const string DictionaryName = "Dictionary";
    private const string ItemsName = "Items[]";
    private const string KeysName = "Keys[]";
    private const string ValuesName = "Values[]";
    private const string CountName = "Count";
    #endregion // Field

    #region Event
    /// <summary>
    ///   項目が追加、削除、変更、移動された場合、またはリスト全体が更新されたときに発生します。
    /// </summary>
    public event NotifyCollectionChangedEventHandler CollectionChanged;

    /// <summary>
    ///   プロパティ値が変更されたときに発生します。
    /// </summary>
    protected virtual event PropertyChangedEventHandler PropertyChanged;

    event PropertyChangedEventHandler INotifyPropertyChanged.PropertyChanged
    {
      add
      {
        PropertyChanged += value;
      }
      remove
      {
        PropertyChanged -= value;
      }
    }

    /// <summary>
    ///   指定された引数を使用して、
    ///   <see cref="E:System.Collections.Generic.ObservableDictionary`2.PropertyChanged" /> イベントを発生させます。
    /// </summary>
    /// <param name="propertyName">変更されたプロパティの名前。</param>
    protected virtual void OnPropertyChanged(string propertyName)
    {
      if (PropertyChanged != null)
      {
        PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
      }
    }

    /// <summary>
    ///   指定された引数を使用して、
    ///   <see cref="E:System.Collections.Generic.ObservableDictionary`2.CollectionChanged" /> イベントを発生させます。
    /// </summary>
    /// <param name="e">発生させるイベントの引数。</param>
    protected virtual void OnCollectionChanged(NotifyCollectionChangedEventArgs e)
    {
      if (CollectionChanged != null)
      {
        CollectionChanged(this, e);
      }
    }

    private void OnCollectionAdded(KeyValuePair<TKey, TValue> changedItem, int startingIndex)
    {
      OnPropertyChanged(DictionaryName);
      OnPropertyChanged(ItemsName);
      OnPropertyChanged(CountName);
      OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, changedItem, startingIndex));
    }

    private void OnCollectionRemoved(KeyValuePair<TKey, TValue> changedItem, int startingIndex)
    {
      OnPropertyChanged(DictionaryName);
      OnPropertyChanged(ItemsName);
      OnPropertyChanged(KeysName);
      OnPropertyChanged(ValuesName);
      OnPropertyChanged(CountName);
      OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, changedItem, startingIndex));
    }

    private void OnCollectionMoved(KeyValuePair<TKey, TValue> changedItem, int index, int oldIndex)
    {
      OnPropertyChanged(DictionaryName);
      OnPropertyChanged(ItemsName);
      OnPropertyChanged(KeysName);
      OnPropertyChanged(ValuesName);
      OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Move, changedItem, index, oldIndex));
    }

    private void OnCollectionReplaced(KeyValuePair<TKey, TValue> newItem, KeyValuePair<TKey, TValue> oldItem)
    {
      OnPropertyChanged(DictionaryName);
      OnPropertyChanged(ItemsName);
      OnPropertyChanged(KeysName);
      OnPropertyChanged(ValuesName);
      OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Replace, newItem, oldItem));
    }

    private void OnCollectionReset()
    {
      OnPropertyChanged(DictionaryName);
      OnPropertyChanged(ItemsName);
      OnPropertyChanged(KeysName);
      OnPropertyChanged(ValuesName);
      OnPropertyChanged(CountName);
      OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
    }
    #endregion // Event

    #region Method
    /// <summary>
    ///   指定したインデックス位置の項目を、指定した項目で置き換えます。
    /// </summary>
    /// <param name="index">置き換えられる項目の 0 から始まるインデックス。</param>
    /// <param name="item">新しい項目。</param>
    protected override void SetItem(int index, KeyValuePair<TKey, TValue> item)
    {
      KeyValuePair<TKey, TValue> oldItem = this[index];
      base.SetItem(index, item);
      OnCollectionReplaced(item, oldItem);
    }

    /// <summary>
    ///   <see cref="T:System.Collections.Generic.ObservableDictionary`2" /> 内の指定したインデックスの位置に要素を挿入します。
    /// </summary>
    /// <param name="index">
    ///   <paramref name="item" /> を挿入する位置の、0 から始まるインデックス番号。
    /// </param>
    /// <param name="item">
    ///   挿入するオブジェクト。
    /// </param>
    /// <exception cref="T:System.ArgumentOutOfRangeException">
    ///   <paramref name="index" /> が 0 未満です。
    ///   または<paramref name="index" /> が <see cref="P:System.Collections.Generic.ObservableDictionary`2Count" /> より大きくなっています。
    /// </exception>
    protected override void InsertItem(int index, KeyValuePair<TKey, TValue> item)
    {
      base.InsertItem(index, item);
      OnCollectionAdded(item, index);
    }

    /// <summary>
    ///   <see cref="T:System.Collections.Generic.ObservableDictionary`2" /> の指定したインデックスにある要素を削除します。
    /// </summary>
    /// <param name="index">削除する要素のインデックス。</param>
    protected override void RemoveItem(int index)
    {
      KeyValuePair<TKey, TValue> removedItem = this[index];
      base.RemoveItem(index);
      OnCollectionRemoved(removedItem, index);
    }

    /// <summary>
    ///   <see cref="T:System.Collections.Generic.ObservableDictionary`2" /> からすべての要素を削除します。
    /// </summary>
    protected override void ClearItems()
    {
      base.ClearItems();
      OnCollectionReset();
    }
    #endregion // Method
  }
}