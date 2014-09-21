/**************************************************************************
*
* File:		BcHtml.cpp
* Author:	Daniel de Zwart
* Ver/Date:	2014/04/14
* Description:
*		Html Creating system
*
*
*
*
**************************************************************************/

#include "BcHtml.h"

BcHtml::BcHtml() :
RootNode_( "html", 0 )
{
}

BcHtmlNode BcHtml::getRootNode()
{
	return BcHtmlNode( &RootNode_ );
}

std::string BcHtml::getHtml()
{
	return RootNode_.getOuterXml();
}

/**************************************************************************
*
* BcHtmlNode implementation
*
*/
BcHtmlNode::BcHtmlNode( BcHtmlNodeInternal* node )
: InternalNode_( node )
{

}

BcHtmlNode::BcHtmlNode( BcHtmlNode& cpy )
{
	InternalNode_ = cpy.InternalNode_;
	NextTag_ = cpy.NextTag_;
}


BcHtmlNode BcHtmlNode::operator[]( BcU32 idx )
{
	if ( idx < InternalNode_->Children.size() )
		return BcHtmlNode( InternalNode_->Children[ idx ] );
	return 0;
}

BcHtmlNode BcHtmlNode::operator[]( std::string tag )
{
	for ( BcU32 Idx = 0; Idx < InternalNode_->Children.size(); ++Idx )
	{
		if ( InternalNode_->Children[ Idx ]->Tag_ == tag )
		{
			return BcHtmlNode( InternalNode_->Children[ Idx ] );
		}
	}
	return 0;/**/
}


BcHtmlNode BcHtmlNode::createChildNode( std::string tag )
{
	BcHtmlNodeInternal* ret = InternalNode_->createChildNode( tag );
	return BcHtmlNode( ret );
}

std::string BcHtmlNode::getTag()
{
	return InternalNode_->getTag();
}

std::string BcHtmlNode::getContents()
{
	return InternalNode_->getContents();
}

BcHtmlNode& BcHtmlNode::setAttribute( std::string attr, std::string value )
{
	InternalNode_->setAttribute( attr, value );
	return *this;
}

BcHtmlNode& BcHtmlNode::setTag( std::string tag )
{
	InternalNode_->setTag( tag );
	return *this;
}

BcHtmlNode& BcHtmlNode::setContents( std::string contents )
{
	InternalNode_->setContents( contents );
	return *this;
}

std::string BcHtmlNode::getOuterXml()
{
	return InternalNode_->getOuterXml();
}

bool BcHtmlNode::operator == ( const BcHtmlNode& v )
{
	return ( v.InternalNode_ == InternalNode_ );
}

BcHtmlNode BcHtmlNode::NextSiblingNode()
{
	if ( InternalNode_->Parent_ == 0 )
		return BcHtmlNode( 0 );
	BcU32 Idx;
	for ( Idx = 0; Idx < InternalNode_->Parent_->Children.size(); ++Idx )
	{
		if ( InternalNode_->Parent_->Children[ Idx ] == InternalNode_ )
		{
			break;
		}
	}
	Idx = Idx + 1;
	for ( ; Idx < InternalNode_->Parent_->Children.size(); ++Idx )
	{
		if ( ( InternalNode_->Parent_->Children[ Idx ]->Tag_ == NextTag_ ) || ( NextTag_ == "" ) )
		{
			BcHtmlNode ret( InternalNode_->Parent_->Children[ Idx ] );
			ret.NextTag_ = NextTag_;
			return ret;
		}
	}
	return BcHtmlNode( 0 );
}


/**************************************************************************
*
* BcHtmlNodeInternal implementation
*
*/

BcHtmlNodeInternal::~BcHtmlNodeInternal()
{
	for ( BcU32 Idx = 0; Idx < Children.size(); ++Idx )
		delete Children[ Idx ];
}

BcHtmlNodeInternal::BcHtmlNodeInternal( std::string tag, BcHtmlNodeInternal* parent )
: Tag_( tag ), Parent_( parent )
{

}

BcHtmlNodeInternal* BcHtmlNodeInternal::createChildNode( std::string tag )
{
	Children.push_back( new BcHtmlNodeInternal( tag, this ) );
	return Children[ Children.size() - 1 ];
}

std::string BcHtmlNodeInternal::getTag()
{
	return Tag_;
}

std::string BcHtmlNodeInternal::getContents()
{
	return Contents_;
}

void BcHtmlNodeInternal::setAttribute( std::string attr, std::string value )
{
	Attributes_[ attr ] = value;
}

void BcHtmlNodeInternal::setTag( std::string tag )
{
	Tag_ = tag;
}

void BcHtmlNodeInternal::setContents( std::string contents )
{
	Contents_ = contents;
}

std::string BcHtmlNodeInternal::getOuterXml()
{
	if ( Tag_ == "" )
		return Contents_;
	std::string output = "<" + Tag_ + " ";
	for each ( auto attr in Attributes_ )
	{
		output += attr.first;
		output += "=\"";
		output += attr.second;
		output += "\" ";
	}
	if ( ( Contents_ == "" ) && ( Children.size() == 0 ) &&
		( ( Tag_ == "p" ) || ( Tag_ == "br" ) ) )
	{
		output += ">";
		return output;
	}
	else
	{
		output += ">";
		output += Contents_;
		for each ( BcHtmlNodeInternal* var in Children )
		{
			output += var->getOuterXml();
		}
		output += "</" + Tag_ + ">";
		return output;
	}
	return "";
}
