#include <vector>
#include <list>
#include <sstream>
#include <functional>
#include <math.h>

using namespace std;

template <typename KeyType, typename ValueType>  
class ExpandableHashMap {
    public:
    ExpandableHashMap(double maxload=0.5);
    ~ExpandableHashMap();

    void expand();
    void associate(const KeyType &key, const ValueType &value);
    void reset();
    size_t size() const;
    size_t nbuckets() const;
    const ValueType* find(const KeyType& key) const;

    private:
    unsigned int _hash(const KeyType &key) const
    {
        stringstream ss;
        ss << key;
        return hash<string>()(ss.str()) % m_buckets.size();
    }
    size_t m_occupied;
    size_t m_associations;
    double m_maxload;
    vector< list< pair <const KeyType*, const ValueType*> > *> m_buckets; 
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maxload): m_buckets(8), m_associations(0)
{
    m_maxload = maxload;
    for (int i = 0; i < m_buckets.size(); i++)
    {
        m_buckets[i] = nullptr;
    }
}

template<typename KeyType, typename ValueType>
size_t ExpandableHashMap<KeyType, ValueType>::size() const
{
    return m_associations;
}

template<typename KeyType, typename ValueType>
size_t ExpandableHashMap<KeyType, ValueType>::nbuckets() const
{
    return m_buckets.size();
}
template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
    for (int i = 0; i < m_buckets.size(); i++)
    {
         if(m_buckets[i] != nullptr)
         {
             typename list< pair<const KeyType*, const ValueType*>>::const_iterator li = 
                m_buckets[i]->begin();
             while (li != m_buckets[i]->end())
             {
                 delete li->first;
                 delete li->second;
                 li++;
             }
             delete m_buckets[i];
         }
    }
    m_occupied = 0;
    m_associations = 0;
    m_buckets.clear();
    m_buckets.resize(8);
}
template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    this->reset();
}


template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::expand()
{
    vector<pair<const KeyType, const ValueType> > temp_bucket;
    for (int u = 0; u < m_buckets.size(); u++)
    {
        if (m_buckets[u] == nullptr)
        {
            continue;
        }
        else
        {
            typename list< pair<const KeyType*, const ValueType*>>::const_iterator li = 
                m_buckets[u]->begin();
            while (li != m_buckets[u]->end())
            {

                temp_bucket.push_back(pair<KeyType, ValueType>(*(li->first), *(li->second)));
                delete li->first;
                delete li->second;
                li++;
            }
        }
    }
    size_t sz = m_buckets.size() * 2;
    m_buckets.clear();
    m_buckets.resize(sz);
    m_occupied = 0;
    m_associations = 0;
    for (int i = 0; i < temp_bucket.size(); i++)
    {
        associate(temp_bucket[i].first, temp_bucket[i].second);
    }
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    double sz = m_buckets.size();
    if ((double)m_occupied/sz > m_maxload)
    {
        expand();
    }
    int index = _hash(key);
    if (m_buckets[index] == nullptr)
    {
        m_buckets[index] = new list<pair<const KeyType*,const ValueType*> >;
        m_occupied++;
    }
    else
    {
        typename list< pair<const KeyType*, const ValueType*>>::iterator li = 
            m_buckets[index]->begin();
        while (li != m_buckets[index]->end())
        {
            if (key == *(li->first))
            {
                delete li->first;
                delete li->second;
                m_buckets[index]->erase(li);
                m_associations--;
                break;
            }
            li++;
        }
    }
    KeyType* nkey = new KeyType;
    ValueType* nvalue = new ValueType;
    *nkey = key;
    *nvalue = value;
    pair<const KeyType*, const ValueType*> p = make_pair(nkey, nvalue);
    m_buckets[index]->push_front(p);
    m_associations++;
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
    int index = _hash(key);

    if (m_buckets[index] == nullptr)
    {
        return nullptr;
    }
  
    typename list< pair<const KeyType*, const ValueType*>>::const_iterator li = m_buckets[index]->begin();
    while (li != m_buckets[index]->end())
    {
        if (*(li->first) == key)
        {
            return li->second;
        }
        li++;
    }
    return nullptr;
}
