///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 DreamWorks Animation LLC. 
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// *       Redistributions of source code must retain the above
//         copyright notice, this list of conditions and the following
//         disclaimer.
// *       Redistributions in binary form must reproduce the above
//         copyright notice, this list of conditions and the following
//         disclaimer in the documentation and/or other materials
//         provided with the distribution.
// *       Neither the name of DreamWorks Animation nor the names of its
//         contributors may be used to endorse or promote products
//         derived from this software without specific prior written
//         permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////
///
/// @file DcxPixel.h

#ifndef INCLUDED_DCX_PIXEL_H
#define INCLUDED_DCX_PIXEL_H

//-----------------------------------------------------------------------------
//
//  class  Pixel
//
//-----------------------------------------------------------------------------

#include "DcxChannelSet.h"

#ifdef __ICC
// disable icc remark #1572: 'floating-point equality and inequality comparisons are unreliable'
//   this is coming from OpenEXR/half.h...
#   pragma warning(disable:1572)
#endif
#include <OpenEXR/half.h>  // For Pixelh

#include <string.h> // for memset in some compilers

OPENDCX_INTERNAL_NAMESPACE_HEADER_ENTER


//-------------------------------------------------------------------------------------
//
//  class Pixel
//
//      Contains a fixed-size array of data and a ChannelSet
//      defining the active channels.
//
//      Intentionally similar to Nuke's DD::Image::Pixel class.
//
//      NOTE: This class is solely intended for use in image processing algorithms and
//      *not* for multi-pixel data storage (ex. a line or tile's worth.)
//
//      (TODO: is it worth it to change this class to a packed-array?
//       Managing the data packing/unpacking may be far more trouble than it's worth)
//
//-------------------------------------------------------------------------------------

template <class T>
class DCX_EXPORT Pixel
{
  public:
    ChannelSet  channels;                   // Set of enabled channels
    T           chan[Dcx::Chan_Max+1];      // Fixed-sized array of channel values


  public:

    //-------------------------------------------------
    // Assigns channel mask but leaves junk in channels
    //-------------------------------------------------

    Pixel (const ChannelSet&);


    //-----------------------------------------------------------------
    // Assigns channel mask and sets enabled channels to a single value
    //-----------------------------------------------------------------

    Pixel (const ChannelSet&, T val);


    //-----------------
    // Copy constructor
    //-----------------

    Pixel (const Pixel&);


    //-----------------------------
    // Set channels to zero (black)
    //-----------------------------

    void    erase ();
    void    erase (const ChannelSet&);
    void    erase (ChannelIdx);


    //---------------------------
    // Replace values in channels
    //---------------------------

    void    replace (const Pixel&,
                     const ChannelSet&);
    void    replace (const Pixel&);


    //-------------------------------------------
    // Pointer to beginning of channel data array
    //-------------------------------------------

    T* array();


    //----------------
    // Channel access
    //----------------

    T& operator [] (ChannelIdx);
    T& operator [] (ChannelSet::iterator);
    const T& operator [] (ChannelIdx) const;
    const T& operator [] (ChannelSet::iterator) const;


    //-----------
    // Assignment
    //-----------

    Pixel& operator = (const Pixel&);
    Pixel& operator = (T val);
    void set(ChannelIdx, T val);
    void set(const ChannelSet&, T val);
    void set(T val);

    //---------
    // Multiply
    //---------

    Pixel  operator *  (const Pixel&) const;
    Pixel& operator *= (const Pixel&);
    Pixel  operator *  (T val) const;
    Pixel& operator *= (T val);


    //-------
    // Divide
    //-------

    Pixel  operator /  (const Pixel&) const;
    Pixel& operator /= (const Pixel&);
    Pixel  operator /  (T val) const;
    Pixel& operator /= (T val);


    //---------
    // Addition
    //---------

    Pixel  operator +  (const Pixel&) const;
    Pixel& operator += (const Pixel&);
    Pixel  operator +  (T val) const;
    Pixel& operator += (T val);

    //------------
    // Subtraction
    //------------

    Pixel  operator -  (const Pixel&) const;
    Pixel& operator -= (const Pixel&);
    Pixel  operator -  (T val) const;
    Pixel& operator -= (T val);

};

// Predefined types:
typedef Pixel<int32_t>  Pixeli;
typedef Pixel<uint32_t> Pixelu;  // Imf::UINT
typedef Pixel<half>     Pixelh;  // Imf::HALF
typedef Pixel<float>    Pixelf;  // Imf::FLOAT
typedef Pixel<double>   Pixeld;






//-----------------
// Inline Functions
//-----------------

template <class T>
inline Pixel<T>::Pixel(const ChannelSet& set) : channels(set) {}
template <class T>
inline Pixel<T>::Pixel(const ChannelSet& set, T val) : channels(set) { this->set(val); }
template <class T>
inline void Pixel<T>::erase() { memset(chan, 0, sizeof(T)*Dcx::Chan_Max); }
template <class T>
inline void Pixel<T>::erase(const ChannelSet& set)
{
    foreach_channel(z, set)
        chan[*z] = 0;
}
template <class T>
inline void Pixel<T>::erase(ChannelIdx channel) { chan[channel] = 0; }
//---------------------------------------------------
template <class T>
inline void Pixel<T>::replace(const Pixel<T>& b, const ChannelSet& set)
{
    foreach_channel(z, set)
        chan[*z] = b.chan[*z];
}

template <class T>
inline void Pixel<T>::replace(const Pixel<T>& b)
{
    foreach_channel(z, b.channels)
        chan[*z] = b.chan[*z];
}
template <class T>
inline Pixel<T>& Pixel<T>::operator = (const Pixel<T>& b) { channels = b.channels; this->replace(b); return *this; }
template <class T>
inline Pixel<T>::Pixel(const Pixel<T>& b) { *this = b; }
//---------------------------------------------------
template <class T>
inline T& Pixel<T>::operator [] (ChannelIdx channel) { return chan[channel]; }
template <class T>
inline const T& Pixel<T>::operator [] (ChannelIdx channel) const { return chan[channel]; }
template <class T>
inline T& Pixel<T>::operator [] (ChannelSet::iterator z) { return chan[*z]; }
template <class T>
inline const T& Pixel<T>::operator [] (ChannelSet::iterator z) const { return chan[*z]; }
template <class T>
inline T* Pixel<T>::array() { return chan; }
//---------------------------------------------------
template <class T>
inline Pixel<T>& Pixel<T>::operator = (T val)
{
    foreach_channel(z, channels)
        chan[*z] = val;
    return *this;
}
template <class T>
inline void Pixel<T>::set(ChannelIdx channel, T val) { chan[channel] = val; }
template <class T>
inline void Pixel<T>::set(T val)
{
    foreach_channel(z, channels)
        chan[*z] = val;
}
template <class T>
inline void Pixel<T>::set(const ChannelSet& _set, T val)
{
    channels = _set;
    this->set(val);
}
//---------------------------------------------------
template <class T>
inline Pixel<T> Pixel<T>::operator * (T val) const
{
    Pixel<T> ret(channels);
    foreach_channel(z, channels)
        ret.chan[*z] = chan[*z] * val;
    return ret;
}
template <class T>
inline Pixel<T>& Pixel<T>::operator *= (T val)
{
    foreach_channel(z, channels)
        chan[*z] *= val;
    return *this;
}
template <class T>
inline Pixel<T> Pixel<T>::operator * (const Pixel<T>& b) const
{
    Pixel<T> ret(channels);
    foreach_channel(z, b.channels)
        ret.chan[*z] = (chan[*z] * b.chan[*z]);
    return ret;
}
template <class T>
inline Pixel<T>& Pixel<T>::operator *= (const Pixel<T>& b)
{
    foreach_channel(z, b.channels)
        chan[*z] *= b.chan[*z];
    return *this;
}
//---------------------------------------------------
template <class T>
inline Pixel<T> Pixel<T>::operator / (T val) const
{
    const T ival = (T)1 / val;
    Pixel<T> ret(channels);
    foreach_channel(z, channels)
        ret.chan[*z] = chan[*z]*ival;
    return ret;
}
template <class T>
inline Pixel<T>& Pixel<T>::operator /= (T val)
{
    const T ival = (T)1 / val;
    foreach_channel(z, channels)
        chan[*z] *= ival;
    return *this;
}
template <class T>
inline Pixel<T> Pixel<T>::operator / (const Pixel<T>& b) const
{
    Pixel<T> ret(channels);
    foreach_channel(z, b.channels)
        ret.chan[*z] = (chan[*z] / b.chan[*z]);
    return ret;
}
template <class T>
inline Pixel<T>& Pixel<T>::operator /= (const Pixel<T>& b)
{
    foreach_channel(z, b.channels)
        chan[*z] /= b.chan[*z];
    return *this;
}
//---------------------------------------------------
template <class T>
inline Pixel<T> Pixel<T>::operator + (T val) const
{
    Pixel<T>ret(channels);
    foreach_channel(z, channels)
        ret.chan[*z] = (chan[*z] + val);
    return ret;
}
template <class T>
inline Pixel<T>& Pixel<T>::operator += (T val)
{
    foreach_channel(z, channels)
        chan[*z] += val;
    return *this;
}
template <class T>
inline Pixel<T> Pixel<T>::operator + (const Pixel<T>& b) const
{
    Pixel<T>ret(channels);
    foreach_channel(z, b.channels)
        ret.chan[*z] = (chan[*z] + b.chan[*z]);
    return ret;
}
template <class T>
inline Pixel<T>& Pixel<T>::operator += (const Pixel<T>& b)
{
    foreach_channel(z, b.channels)
        chan[*z] += b.chan[*z];
    return *this;
}
//---------------------------------------------------
template <class T>
inline Pixel<T> Pixel<T>::operator - (T val) const
{
    Pixel<T> ret(channels);
    foreach_channel(z, channels)
        ret.chan[*z] = (chan[*z] - val);
    return ret;
}
template <class T>
inline Pixel<T>& Pixel<T>::operator -= (T val)
{
    foreach_channel(z, channels)
        chan[*z] -= val;
    return *this;
}
template <class T>
inline Pixel<T> Pixel<T>::operator - (const Pixel<T>& b) const
{
    Pixel<T> ret(channels);
    foreach_channel(z, b.channels)
        ret.chan[*z] = (chan[*z] - b.chan[*z]);
    return ret;
}
template <class T>
inline Pixel<T>& Pixel<T>::operator -= (const Pixel<T>& b)
{
    foreach_channel(z, b.channels)
        chan[*z] -= b.chan[*z];
    return *this;
}


OPENDCX_INTERNAL_NAMESPACE_HEADER_EXIT

#endif // INCLUDED_DCX_PIXEL_H