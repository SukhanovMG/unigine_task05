#include <cstring>
#include <cstdint>
#include <iostream>

using namespace std;

// STRINGS

class unowned_string
{
public:
    unowned_string()
        : m_str(nullptr), m_len(0)
    {
    }
    unowned_string(const char *str, unsigned len)
        : m_str(str), m_len(len)
    {
    }
    unowned_string(const char *str)
        : m_str(str), m_len(strlen(str))
    {
    }
    ~unowned_string() {}
    const char *c_str() const { return m_str; }
    unsigned size() const { return m_len; }

private:
    const char *m_str;
    unsigned m_len;
};

class mystring
{
public:
    mystring()
        : m_str(nullptr), m_len(0)
    {
    }
    mystring(const char* str, unsigned len)
    {
        init_from_buf(str, len);
    }
    mystring(const unowned_string& uo_str)
    {
        init_from_buf(uo_str.c_str(), uo_str.size());
    }
    mystring(const string& str)
    {
        init_from_buf(str.c_str(), str.size());
    }
    mystring& operator=(const string& str)
    {
        init_from_buf(str.c_str(), str.size());
        return *this;
    }
    mystring& operator=(const unowned_string& uo_str)
    {
        init_from_buf(uo_str.c_str(), uo_str.size());
        return *this;
    }
    bool operator==(const unowned_string& rhs)
    {
        return strncmp(m_str, rhs.c_str(), rhs.size()) == 0;
    }
    bool operator!=(const unowned_string& rhs)
    {
        return !(*this == rhs);
    }
    ~mystring()
    {
        delete[] m_str;
    }

    const char *c_str() const { return m_str; }
    unsigned size() const { return m_len; }
private:
    char* m_str;
    unsigned m_len;

    void init_from_buf(const char* buf, unsigned len)
    {
        m_str = new char[len+1];
        strncpy(m_str, buf, len);
        m_str[len] = '\0';
        m_len = len;
    }
};

// TRIE

static const unsigned alphabet_size = 26;

inline unsigned index_by_char(char c)
{
    return (unsigned) (c - 'a');
}

inline char char_by_index(unsigned idx)
{
    return ((char) idx) + 'a';
}

struct TrieNode
{
    TrieNode(TrieNode* parent = nullptr) : m_childs(), m_parent(parent), m_value(0) {}
    TrieNode* m_childs[alphabet_size];
    TrieNode* m_parent;
    unsigned m_value;
};

class Trie_iterator
{
public:
    Trie_iterator(unsigned level_count, TrieNode *root)
        : m_len(0), m_node(root)
    {
        m_key_buf = new char[level_count]();

        for (unsigned level = 0; level < level_count; level++)
        {
            unsigned idx = find_non_null_child(m_node);
            if (idx == alphabet_size)
                break;
            m_key_buf[level] = char_by_index(idx);
            m_len++;
            if (get_value() > 0)
                break;
            m_node = m_node->m_childs[idx];
        }
    }
    ~Trie_iterator()
    {
        delete[] m_key_buf;
    }

    void next()
    {
        if (m_node == nullptr)
            return;
        if (m_len == 0)
            return;

        unsigned cur_idx = index_by_char(m_key_buf[m_len-1]);
        if (cur_idx == alphabet_size)
        {
            m_node = m_node->m_parent;
            m_len--;
        }
        if (m_node->m_childs[cur_idx])
        {
            m_node = m_node->m_childs[cur_idx];
            m_len++;
            m_key_buf[m_len-1] = 'a';
            return;
        }
        m_key_buf[m_len-1]++;
    }

    Trie_iterator& operator++()
    {
        next();
        while (m_node != nullptr && get_value() == 0)
            next();
        return *this;
    }

    unowned_string get_key() const { return unowned_string(m_key_buf, m_len); }
    //unsigned get_value() const { return m_value; }
    unsigned get_value() const
    {
        if (!m_node) return 0;
        if (m_len == 0) return 0;
        unsigned cur_idx = index_by_char(m_key_buf[m_len-1]);
        return m_node->m_childs[cur_idx] == nullptr ? 0 : m_node->m_childs[cur_idx]->m_value;
    }
private:
    unsigned find_non_null_child(TrieNode * node, unsigned start = 0)
    {
        if (!node)
            return alphabet_size;
        for (unsigned i = start; i < alphabet_size; i++)
        {
            if (node->m_childs[i])
                return i;
        }
        return alphabet_size;
    }

    char * m_key_buf;
    unsigned m_len;
    TrieNode * m_node;
};


class Trie
{
public:
    Trie() : m_root(), m_count(0), m_levels(0) {}

    unsigned& operator[](const unowned_string& uo_str);

    Trie_iterator begin() { return Trie_iterator(m_levels, &m_root); }
private:
    TrieNode m_root;
    unsigned m_count;
    unsigned m_levels;
};

unsigned& Trie::operator[](const unowned_string& uo_str)
{
    TrieNode* pnode = &m_root;

    for (unsigned level = 0; level < uo_str.size(); level++)
    {
        unsigned child_idx = index_by_char(uo_str.c_str()[level]);
        if (!pnode->m_childs[child_idx])
            pnode->m_childs[child_idx] = new TrieNode(pnode);
        pnode = pnode->m_childs[child_idx];
    }

    if (uo_str.size() > m_levels)
        m_levels = uo_str.size();

    return pnode->m_value;
}

int main()
{
    Trie t;

    const char *str1 = "text";
    const char *str2 = "test";
    const char *str3 = "tes";

    unowned_string uo_str1 = str1;
    unowned_string uo_str2 = str2;
    unowned_string uo_str3 = str3;

    t[uo_str1] = 1;
    t[uo_str2] = 2;
    t[uo_str3] = 3;

    Trie_iterator tit = t.begin();

    cout.write(tit.get_key().c_str(), tit.get_key().size()); cout << " : " << tit.get_value() << endl;
    ++tit;
    cout.write(tit.get_key().c_str(), tit.get_key().size()); cout << " : " << tit.get_value() << endl;
    ++tit;
    cout.write(tit.get_key().c_str(), tit.get_key().size()); cout << " : " << tit.get_value() << endl;

    // cout.write(uo_str1.c_str(), uo_str1.size()); cout << " : " << t[uo_str1] << endl;
    // cout.write(uo_str2.c_str(), uo_str2.size()); cout << " : " << t[uo_str2] << endl;

    return 0;
}