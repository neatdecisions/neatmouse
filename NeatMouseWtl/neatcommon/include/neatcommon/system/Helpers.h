//
// Copyright © 2016 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include <sstream>


namespace neatcommon {
namespace system {

bool wstring2string(const std::wstring & wVal, std::string & outValue);
bool string2wstring(const std::string & value, std::wstring & outValue);
bool wstring2utf8string(const std::wstring & value, std::string & outValue);


//-----------------------------------------------------------------------------
template < class T > 
inline std::wstring to_string ( const T & t )
{
	std::wstringstream ss;
	ss << t;
	std::wstring res( ss.str() );
	return res;
}

//----------------------------------------------------------------------------
template < class T > 
inline T from_string_def( const std::wstring & s, T def )
{
	T result;
	std::wstringstream ss(s);
	try
	{
		ss >> result;
		if (ss.fail())
			result = def;
	} catch ( ... )
	{
		return def;
	}
	return result;
}


//----------------------------------------------------------------------------
struct ProductInfo
{
	unsigned short int major;
	unsigned short int minor;
	unsigned short int build1;
	unsigned short int build2;

	std::string ToMultibyte()
	{
		std::stringstream stream;
		stream << major << '.' << minor << '.' << build1 << '.' << build2;
		return stream.str();
	}

	std::wstring ToUnicode()
	{
		std::wstringstream stream;
		stream << major << L'.' << minor << L'.' << build1 << L'.' << build2;
		return stream.str();
	}
};

bool GetProductVersion(ProductInfo & info);
bool FileExists(const std::wstring & fileName);
BOOL IsWindowsVistaOrLater();
std::wstring GetFileName(const std::wstring & fileName);
bool FileExists(const std::wstring & fileName);
std::vector<std::string> SplitString(const std::string & s, const std::string & delimeter);

}}