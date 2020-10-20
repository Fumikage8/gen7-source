using System.Collections.ObjectModel;
using System.Linq;

namespace System.Collections.Generic
{
  public class CollectionBasedDictionary<TKey, TValue>
      : KeyedCollection<TKey, KeyValuePair<TKey, TValue>>, IDictionary<TKey, TValue>
  {
    #region Constructor
    /// <summary>
    ///   空で、既定の初期量を備え、キーの型の既定の等値比較子を使用する、
    ///   <see cref="T:System.Collections.Generic.CollectionBasedDictionary`2" /> クラスの
    ///   新しいインスタンスを初期化します。
    /// </summary>
    public CollectionBasedDictionary()
      : base()
    {
    }

    /// <summary>
    ///   指定した <see cref="T:System.Collections.Generic.IDictionary`2" /> から要素をコピーして格納し、
    ///   キーの型の既定の等値比較子を使用する、
    ///   <see cref="T:System.Collections.Generic.CollectionBasedDictionary`2" /> クラスの
    ///   新しいインスタンスを初期化します。
    /// </summary>
    /// <param name="dictionary">
    ///   新しい <see cref="T:System.Collections.Generic.CollectionBasedDictionary`2" /> に要素がコピーされた 
    ///   <see cref="T:System.Collections.Generic.IDictionary`2" />。
    /// </param>
    /// <exception cref="T:System.ArgumentNullException">
    ///   <paramref name="dictionary" /> が null です。
    /// </exception>
    /// <exception cref="T:System.ArgumentException">
    ///   <paramref name="dictionary" /> に、1 つ以上の重複するキーが格納されています。
    /// </exception>
    public CollectionBasedDictionary(IDictionary<TKey, TValue> dictionary)
      : base()
    {
      foreach (var kvp in dictionary)
      {
        Add(kvp);
      }
    }

    /// <summary>
    ///   空で、既定の初期量を備え、指定した <see cref="T:System.Collections.Generic.IEqualityComparer`1" /> を使用する、
    ///   <see cref="T:System.Collections.Generic.CollectionBasedDictionary`2" /> クラスの
    ///   新しいインスタンスを初期化します。
    /// </summary>
    /// <param name="comparer">
    ///   キーの比較時に使用する <see cref="T:System.Collections.Generic.IEqualityComparer`1" /> 実装。
    ///   キーの型の既定の <see cref="T:System.Collections.Generic.EqualityComparer`1" /> を使用する場合は null。
    /// </param>
    public CollectionBasedDictionary(IEqualityComparer<TKey> comparer)
      : base(comparer)
    {
    }

    /// <summary>
    ///   指定した <see cref="T:System.Collections.Generic.IDictionary`2" /> から要素をコピーして格納し、
    ///   指定した <see cref="T:System.Collections.Generic.IEqualityComparer`1" /> を使用する、
    ///   <see cref="T:System.Collections.Generic.CollectionBasedDictionary`2" /> クラスの
    ///   新しいインスタンスを初期化します。
    /// </summary>
    /// <param name="dictionary">
    ///   新しい <see cref="T:System.Collections.Generic.CollectionBasedDictionary`2" /> に要素がコピーされた
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
    public CollectionBasedDictionary(IDictionary<TKey, TValue> dictionary, IEqualityComparer<TKey> comparer)
      : base(comparer)
    {
      foreach (var kvp in dictionary)
      {
        Add(kvp);
      }
    }
    #endregion // Constructor

    #region KeyedCollection<,>
    /// <summary>
    ///   指定した要素からキーを抽出します。
    /// </summary>
    /// <returns>指定した要素のキー。</returns>
    /// <param name="item">キーの抽出元要素。</param>
    protected override TKey GetKeyForItem(KeyValuePair<TKey, TValue> item)
    {
      return item.Key;
    }
    #endregion // KeyedCollection<,>

    #region IDictionary<>
    /// <summary>
    ///   <see cref="T:System.Collections.Generic.CollectionBasedDictionary`2" /> 内のキーを
    ///   格納しているコレクションを取得します。
    /// </summary>
    public ICollection<TKey> Keys
    {
      get
      {
        if (Dictionary == null)
        {
          // 辞書が未生成の場合は空配列を返す。
          return Enumerable.Empty<TKey>().ToArray();
        }
        return Dictionary.Keys;
      }
    }

    /// <summary>
    ///   <see cref="T:System.Collections.Generic.CollectionBasedDictionary`2" /> 内の値を
    ///   格納しているコレクションを取得します。
    /// </summary>
    public ICollection<TValue> Values
    {
      get
      {
        if (Dictionary == null)
        {
          // 辞書が未生成の場合は空配列を返す。
          return Enumerable.Empty<TValue>().ToArray();
        }
        // KeyValuePair[] から Value[] に変換する。
        return Dictionary.Values
            .Select(x => x.Value)
            .ToArray();
      }
    }

    /// <summary>
    ///   指定したキーに関連付けられている値を取得または設定します。
    /// </summary>
    /// <returns>
    ///   指定したキーに関連付けられている値。指定したキーが見つからなかった場合、
    ///   get 操作は <see cref="T:System.Collections.Generic.KeyNotFoundException" /> をスローし、
    ///   set 操作は指定したキーを持つ新しい要素を作成します。
    /// </returns>
    /// <param name="key">取得または設定する値のキー。</param>
    /// <exception cref="T:System.ArgumentNullException">
    ///   <paramref name="key" /> が null です。
    /// </exception>
    /// <exception cref="T:System.Collections.Generic.KeyNotFoundException">
    ///   プロパティが取得されましたが、コレクション内に <paramref name="key" /> が存在しません。
    /// </exception>
    public new TValue this[TKey key]
    {
      get
      {
        if (Dictionary == null)
        {
          throw new KeyNotFoundException("指定されたキーはディクショナリ内に存在しませんでした。");
        }
        return Dictionary[key].Value;
      }
      set
      {
        // KeyValuePair を作成して追加 or 置換する。
        KeyValuePair<TKey, TValue> newValue = new KeyValuePair<TKey, TValue>(key, value);
        KeyValuePair<TKey, TValue> oldValue;
        if (Dictionary.TryGetValue(key, out oldValue))
        {
          var index = Items.IndexOf(oldValue);
          SetItem(index, newValue);
        }
        else
        {
          InsertItem(Count, newValue);
        }
      }
    }

    /// <summary>
    ///   指定したキーと値をディクショナリに追加します。
    ///   </summary>
    /// <param name="key">追加する要素のキー。</param>
    /// <param name="value">追加する要素の値。参照型の場合、null の値を使用できます。</param>
    /// <exception cref="T:System.ArgumentNullException">
    ///   <paramref name="key" /> が null です。
    /// </exception>
    /// <exception cref="T:System.ArgumentException">
    ///   同じキーを持つ要素が、<see cref="T:System.Collections.Generic.CollectionBasedDictionary`2" /> に既に存在します。
    /// </exception>
    public void Add(TKey key, TValue value)
    {
      // KeyValuePair を作成して追加する。
      var kvp = new KeyValuePair<TKey, TValue>(key, value);
      Add(kvp);
    }

    /// <summary>
    ///   <see cref="T:System.Collections.Generic.CollectionBasedDictionary`2" /> に特定の値が格納されているかどうかを判断します。
    /// </summary>
    /// <returns>
    ///   指定した値を持つ要素が <see cref="T:System.Collections.Generic.CollectionBasedDictionary`2" /> に格納されている場合は true。
    ///   それ以外の場合は false。
    /// </returns>
    /// <param name="value">
    ///   <see cref="T:System.Collections.Generic.CollectionBasedDictionary`2" /> 内で検索される値。
    ///   参照型の場合、null の値を使用できます。
    /// </param>
    public bool ContainsKey(TKey key)
    {
      return Dictionary.ContainsKey(key);
    }

    /// <summary>
    ///   指定したキーに関連付けられている値を取得します。
    /// </summary>
    /// <returns>
    ///   指定したキーを持つ要素が <see cref="T:System.Collections.Generic.CollectionBasedDictionary`2" /> に格納されている場合は true。
    ///   それ以外の場合は false。
    /// </returns>
    /// <param name="key">取得する値のキー。</param>
    /// <param name="value">
    ///   このメソッドから制御が戻るときに、キーが見つかった場合は、
    ///   指定したキーに関連付けられている値が格納されます。
    ///   それ以外の場合は <paramref name="value" /> パラメーターの型に対する既定の値。
    ///   このパラメーターは初期化せずに渡されます。
    /// </param>
    /// <exception cref="T:System.ArgumentNullException">
    ///   <paramref name="key" /> が null です。
    /// </exception>
    public bool TryGetValue(TKey key, out TValue value)
    {
      KeyValuePair<TKey, TValue> kvp;
      if (Dictionary == null)
      {
        value = default(TValue);
        return false;
      }

      bool result = Dictionary.TryGetValue(key, out kvp);

      // 取得した KeyValuePair から Value を抜き出す。
      value = kvp.Value;
      return result;
    }
    #endregion // IDictionary<>
  }
}