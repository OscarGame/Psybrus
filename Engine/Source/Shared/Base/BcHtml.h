/**************************************************************************
*
* File:		BcHtml.h
* Author: 	Daniel de Zwart
* Ver/Date:
* Description:
*		Html Creating system
*
*
*
*
**************************************************************************/

#ifndef __BCHTML_H__
#define __BCHTML_H__

#include "Base/BcTypes.h"

#include <map>
#include <vector>
#include <string>
class BcHtml;
class BcHtmlNodeInternal;
//////////////////////////////////////////////////////////////////////////
// BcHtmlNode
class BcHtmlNode
{
	friend BcHtml;
public:
	BcHtmlNode(BcHtmlNode& cpy);
	BcHtmlNode createChildNode(std::string tag);
	std::string getTag();
	std::string getContents();

	BcHtmlNode& setTag(std::string tag);
	BcHtmlNode& setContents(std::string contents);
	BcHtmlNode& setAttribute(std::string attr, std::string value);
	BcHtmlNode operator[](BcU32 idx);
	BcHtmlNode operator[](std::string tag);
	std::string getOuterXml();
	bool operator=(const int&v);
	BcHtmlNode NextSiblingNode();
private:
	BcHtmlNode(BcHtmlNodeInternal* node);
	BcHtmlNodeInternal* InternalNode_;
	std::string NextTag_;
};

//////////////////////////////////////////////////////////////////////////
// BcHtmlNodeInternal
class BcHtmlNodeInternal
{
	friend BcHtmlNode;
	friend BcHtml;
public:
	BcHtmlNodeInternal* createChildNode(std::string tag);
	std::string getTag();
	std::string getContents();

	void setTag(std::string tag);
	void setContents(std::string contents);
	void setAttribute(std::string attr, std::string value);
	BcHtmlNodeInternal* operator[](BcU32 idx);
	BcHtmlNodeInternal* operator[](std::string tag);
	std::string getOuterXml();

	~BcHtmlNodeInternal();
private:
	BcHtmlNodeInternal(std::string tag, BcHtmlNodeInternal* parent);
	std::map<std::string, std::string> Attributes_;
	std::string Tag_;
	std::vector<BcHtmlNodeInternal*> Children;
	std::string Contents_;
	BcHtmlNodeInternal* Parent_;
};


//////////////////////////////////////////////////////////////////////////
// BcHtml
class BcHtml
{
public:
	BcHtml();
	BcHtmlNode getRootNode();
	std::string getHtml();
private:
	/**
	* Private write using va_list.
	*/
	BcHtmlNodeInternal RootNode_;
private:

};


#endif