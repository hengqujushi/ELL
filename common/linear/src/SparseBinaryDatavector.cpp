// SparseBinaryDataVector.cpp

#include "SparseBinaryDataVector.h"
#include <cassert>

namespace linear
{
    template<typename IntegerListType>
    bool SparseBinaryDataVectorBase<IntegerListType>::Iterator::IsValid() const
    {
        return _list_iterator.IsValid();
    }

    template<typename IntegerListType>
    void SparseBinaryDataVectorBase<IntegerListType>::Iterator::Next()
    {
        _list_iterator.Next();
    }

    template<typename IntegerListType>
    uint SparseBinaryDataVectorBase<IntegerListType>::Iterator::GetIndex() const
    {
        return _list_iterator.GetValue();
    }

    template<typename IntegerListType>
    double SparseBinaryDataVectorBase<IntegerListType>::Iterator::GetValue() const
    {
        return (double)1.0;
    }

    template<typename IntegerListType>
    SparseBinaryDataVectorBase<IntegerListType>::Iterator::Iterator(const IndexIteratorType& list_iterator)
        : _list_iterator(list_iterator)
    {}

    template<typename IntegerListType>
    SparseBinaryDataVectorBase<IntegerListType>::SparseBinaryDataVectorBase() 
    {}

    template<typename IntegerListType>
    void SparseBinaryDataVectorBase<IntegerListType>::PushBack(uint index, double value)
    {
        if (value == 0)
        {
            return;
        }

        assert(value == 1);

        _indices.PushBack(index);
    }

    template<typename IntegerListType>
    void SparseBinaryDataVectorBase<IntegerListType>::Reset()
    {
        _indices.Reset();
    }

    //template<typename IntegerListType>
    //void SparseBinaryDataVectorBase<IntegerListType>::foreach_nonzero(function<void(uint, double)> func, uint index_offset) const // TODO
    //{
    //    auto iter = _indices.GetIterator();
    //    while(iter.IsValid())
    //    {
    //        func(index_offset + iter.GetValue(), (double)1.0);
    //        iter.Next();
    //    }
    //}

    template<typename IntegerListType>
    uint SparseBinaryDataVectorBase<IntegerListType>::Size() const
    {
        if(_indices.Size() == 0)
        {
            return 0;
        }
        else
        {
            return _indices.Max() + 1;
        }
    }

    template<typename IntegerListType>
    uint SparseBinaryDataVectorBase<IntegerListType>::NumNonzeros() const
    {
        return _indices.Size();
    }

    template<typename IntegerListType>
    double SparseBinaryDataVectorBase<IntegerListType>::Norm2() const
    {
        return (double)_indices.Size();
    }
    
    template<typename IntegerListType>
    void SparseBinaryDataVectorBase<IntegerListType>::AddTo(double* p_other, double scalar) const
    {
        auto iter = _indices.GetIterator();
        while (iter.IsValid())
        {
            p_other[iter.GetValue()] += scalar;
            iter.Next();
        }
    }

    template<typename IntegerListType>
    double SparseBinaryDataVectorBase<IntegerListType>::Dot(const double* p_other) const
    {
        double value = 0.0;
        
        auto iter = _indices.GetIterator();
        while (iter.IsValid())
        {
            value += (double)p_other[iter.GetValue()];
            iter.Next();
        }
        
        return value;
    }

    template<typename IntegerListType>
    typename SparseBinaryDataVectorBase<IntegerListType>::Iterator SparseBinaryDataVectorBase<IntegerListType>::GetIterator() const
    {
        return Iterator(_indices.GetIterator());
    }

    template<typename IntegerListType>
    void SparseBinaryDataVectorBase<IntegerListType>::Print(ostream & os) const
    {
        auto iterator = GetIterator();
        while(iterator.IsValid())
        {
            auto indexValue = iterator.GetValue();
            os << indexValue << '\t';
            iterator.Next();
        }
    }

    template SparseBinaryDataVectorBase<CompressedIntegerList>;

    IDataVector::type SparseBinaryDataVector::GetType() const
    {
        return type::sparse_binary;
    }

    template SparseBinaryDataVectorBase<IntegerList>;

    IDataVector::type UncompressedSparseBinaryVector::GetType() const
    {
        return type::uncompressed_sparse_binary;
    }
}