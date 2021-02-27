#pragma once
#include <vector>
#include <algorithm>
#include <list>
#include <stdexcept>


template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
    using mapPair = typename std::pair<const KeyType, ValueType>;
    using vectorType = typename std::vector<std::list<mapPair>>;
    using vectorIter = typename std::vector<std::list<mapPair>>::iterator;
    using vectorConstIter = typename std::vector<std::list<mapPair>>::const_iterator;
    using listIter = typename std::list<mapPair>::iterator;
    using listConstIter = typename std::list<mapPair>::const_iterator;
public:
    class iterator {
    private:
        HashMap<KeyType, ValueType, Hash> *ptrOnMap;
        std::pair<vectorIter, listIter> iter;
    public:
        iterator() {}

        iterator(const iterator &other) {
            iter = other.iter;
            ptrOnMap = other.ptrOnMap;
        }

        iterator(const vectorIter &first, const listIter &second, HashMap<KeyType, ValueType, Hash> *ptr) {
            iter = std::make_pair(first, second);
            ptrOnMap = ptr;
        }

        bool operator==(const iterator &other) const {
            return iter == other.iter;
        }

        bool operator!=(const iterator &other) const {
            return !(*this == other);
        }

        listIter operator->() {
            return iter.second;
        }

        const typename listIter::value_type &operator*() {
            return *iter.second;
        }


        iterator &operator++() {
            ++iter.second;
            if (iter.second == iter.first->end()) {
                ++iter.first;
                if (iter.first == ptrOnMap->hashmap.end()) {
                    --iter.first;
                    return *this;
                }
                while (next(iter.first) != ptrOnMap->hashmap.end() && iter.first->begin() == iter.first->end()) {
                    ++iter.first;
                }
                iter.second = iter.first->begin();
                return *this;
            }
            return *this;
        }

        iterator operator++(int) {
            auto copy(*this);
            ++(*this);
            return copy;
        }
    };


    class const_iterator {
    private:
        const HashMap<KeyType, ValueType, Hash> *ptrOnMap;
        std::pair<vectorConstIter, listConstIter> iter;
    public:
        const_iterator() {}

        const_iterator(const const_iterator &other) {
            iter = other.iter;
            ptrOnMap = other.ptrOnMap;
        }

        const_iterator(const vectorConstIter &first, const listConstIter &second,
                       const HashMap<KeyType, ValueType, Hash> *ptr) {
            iter = std::make_pair(first, second);
            ptrOnMap = ptr;
        }

        bool operator==(const const_iterator &other) const {
            return iter == other.iter;
        }

        bool operator!=(const const_iterator &other) const {
            return !(*this == other);
        }

        listConstIter operator->() {
            return iter.second;
        }

        const typename listConstIter::value_type &operator*() {
            return *iter.second;
        }


        const_iterator &operator++() {
            ++iter.second;
            if (iter.second == iter.first->end()) {
                ++iter.first;
                if (iter.first == ptrOnMap->hashmap.end()) {
                    --iter.first;
                    return *this;
                }
                while (next(iter.first) != ptrOnMap->hashmap.end() && iter.first->begin() == iter.first->end()) {
                    ++iter.first;
                }
                iter.second = iter.first->begin();
                return *this;
            }
            return *this;
        }

        const_iterator operator++(int) {
            auto copy(*this);
            ++(*this);
            return copy;
        }
    };


    HashMap() : hasher(Hash()) {
        clear();
    }

    HashMap(Hash _hasher) : hasher(_hasher) {
        clear();
    }


    HashMap(iterator begin, iterator end, Hash _hasher = Hash()) : hasher(_hasher) {
        clear();
        while (begin != end) {
            insert(*begin);
            ++begin;
        }
    }


    HashMap(const std::initializer_list<mapPair> &list, Hash _hasher = Hash()) : hasher(_hasher) {
        clear();
        for (const auto &it : list) {
            insert(it);
        }
    }

    HashMap(const HashMap &other) {
        hasher = other.hasher;
        clear();
        for (const auto &it : other) {
            insert(it);
        }
    }

    HashMap &operator=(const HashMap &other) {
        if (this == &other) {
            return *this;
        }
        this->clear();
        this->hasher = other.hasher;
        for (const auto &it : other) {
            this->insert(it);
        }
        return *this;
    }

    void insert(const mapPair &elem) {
        if (find(elem.first) != end()) {
            return;
        }
        size_t index = hasher(elem.first) % hashmap.size();
        ++count;
        hashmap[index].push_back(elem);
        if (trueBegin > hashmap.begin() + index) {
            trueBegin = hashmap.begin() + index;
        }
        if (count * resizeValue > hashmap.size()) {
            resize(hashmap.size() * 2);
        }
    }

    ValueType &operator[](const KeyType &key) {
        auto it = find(key);
        if (find(key) == end()) {
            insert(std::make_pair(key, ValueType()));
            it = find(key);
        }
        return it->second;
    }

    iterator find(const KeyType &key) {
        size_t index = hasher(key) % hashmap.size();
        for (auto it = hashmap[index].begin(); it != hashmap[index].end(); ++it) {
            if (it->first == key) {
                return iterator(hashmap.begin() + index, it, this);
            }
        }
        return end();
    }

    const_iterator find(const KeyType &key) const {
        size_t index = hasher(key) % hashmap.size();
        for (auto it = hashmap[index].begin(); it != hashmap[index].end(); ++it) {
            if (it->first == key) {
                return const_iterator(hashmap.begin() + index, it, this);
            }
        }
        return end();
    }

    const ValueType &at(const KeyType &key) const {
        auto it = find(key);
        if (find(key) != end()) {
            return it->second;
        }
        throw std::out_of_range("Congratulations! This element is not here :(\n");
    }


    size_t size() const noexcept {
        return count;
    }


    bool empty() const noexcept {
        return count == 0;
    }

    Hash hash_function() const {
        return hasher;
    }

    iterator begin() {
        return iterator(trueBegin, trueBegin->begin(), this);
    }

    iterator end() {
        auto it = prev(hashmap.end());
        return iterator(it, it->end(), this);
    }

    const_iterator begin() const {
        return const_iterator(trueBegin, trueBegin->begin(), this);
    }

    const_iterator end() const {
        auto it = prev(hashmap.end());
        return const_iterator(it, it->end(), this);
    }

    void erase(const KeyType &key) {
        size_t index = hasher(key) % hashmap.size();
        for (auto it = hashmap[index].begin(); it != hashmap[index].end(); ++it) {
            if (it->first == key) {
                hashmap[index].erase(it);
                --count;
                if (count) {
                    while (next(trueBegin) != hashmap.end() && trueBegin->begin() == trueBegin->end()) {
                        ++trueBegin;
                    }
                    if (hashmap.size() > resizeValue * resizeValue * count) {
                        resize(hashmap.size() / resizeValue);
                    }
                } else {
                    clear();
                }
                return;
            }
        }
    }

    void clear(size_t newSize = baseCap) {
        hashmap.clear();
        hashmap.resize(newSize);
        trueBegin = prev(hashmap.end());
        count = 0;
    }

private:
    void resize(size_t newSize) {
        std::vector<mapPair> elements;
        for (const auto &it : (*this)) {
            elements.push_back(it);
        }
        clear(newSize);
        for (const auto &it : elements) {
            insert(it);
        }
    }

    Hash hasher;
    static const size_t baseCap = 10;
    static const size_t resizeValue = 2;
    vectorIter trueBegin;
    size_t count = 0;
    vectorType hashmap;
};



