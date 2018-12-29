//  ***********************************************************************************************
//
//  Lazy Data Library
//
//! @file      lazy_data.h
//! @brief     Defines and implements the class templates Cached, CachingVector, KeyedCache, ...
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   The Boost Licence
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke, Scientific Computing Group at MLZ <j.wuttke@fz-juelich.de>
//
//  ***********************************************************************************************

#ifndef LAZY_DATA_H
#define LAZY_DATA_H

#include <functional>
#include <memory>
#include <vector>

namespace lazy_data {

//! Simple cached object.
template<typename TPayload, typename... TRemakeArgs>
class Cached {
public:
    Cached() = delete;
    Cached(std::function<TPayload(TRemakeArgs...)> f) : remake_(f) {}
    Cached(const Cached&) = delete;
    Cached(Cached&&) = default;
    void invalidate() const { cached_.release(); }
    const TPayload& yield(TRemakeArgs... args) const {
        if (!cached_)
            cached_.reset( new TPayload{remake_(args...)} );
        return *cached_;
    }
    const TPayload* current() const { return cached_ ? cached_.get() : nullptr; }
private:
    mutable std::unique_ptr<TPayload> cached_;
    const std::function<TPayload(TRemakeArgs...)> remake_;
};

//! Caching vector of cached objects.
template<typename TPayload, typename Parent>
class VectorCache {
public:
    VectorCache() = delete;
    VectorCache(const std::function<int()> sizeFunction,
                const std::function<TPayload(const Parent*,int)> remakeOne)
        : sizeFunction_(sizeFunction)
        , remakeOne_([remakeOne](const Parent* p, int i){
                return Cached<TPayload,const Parent*>(
                    [remakeOne,i](const Parent* p)->TPayload{return remakeOne(p,i);}); } )
        {}
    VectorCache(const VectorCache&) = delete;
    VectorCache(VectorCache&&) = default;
    void invalidate() const { data_.clear(); }
    void invalidate_at(int i) const { data_.at(i).invalidate(); }
    int size(const Parent* parent) const {
        check_size(parent);
        return data_.size();
    }
    const TPayload& getget(const Parent* parent, int i) const {
        return get(parent,i).yield(parent); }
    void forAllValids(const Parent* parent, std::function<void(const TPayload& t)> f) const {
        int n = size(parent);
        for (int i=0; i<n; ++i)
            if (const TPayload* d = get(parent,i).current())
                f(*d);
    }
    const std::vector<Cached<TPayload,const Parent*>> &data() const { return data_; }
private:
    const Cached<TPayload,const Parent*>& get(const Parent* parent, int i) const {
        check_size(parent);
        return data_.at(i);
    }
    void check_size(const Parent* parent) const {
        int n = sizeFunction_();
        if (n==data_.size())
            return;
        data_.clear();
        for (int i=0; i<n; ++i)
            data_.push_back(remakeOne_(parent,i));
    }
    mutable std::vector<Cached<TPayload,const Parent*>> data_;
    const std::function<int()> sizeFunction_;
    const std::function<Cached<TPayload,const Parent*>(const Parent*,int)> remakeOne_;
};

//! Cached object with key.
template<typename TPayload, typename TKey>
class KeyedCache {
public:
    void invalidate() const { cached_.release(); }
    const TPayload& get(const TKey key) const {
        if (!cached_ || key!=key_) {
            cached_.reset( new TPayload(key) );
            key_ = key;
        }
        return *cached_;
    }
private:
    mutable std::unique_ptr<TPayload> cached_;
    mutable TKey key_;
};

} // namespace lazy_data

#endif // LAZY_DATA_H
