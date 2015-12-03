/**************************************************************************
*
* File:		ScnModelImport.h
* Author:	Neil Richardson 
* Ver/Date: 06/01/13
* Description:
*		
*		
*
*
* 
**************************************************************************/

#ifndef __SCNMODELIMPORT_H__
#define __SCNMODELIMPORT_H__

#include "System/Content/CsCore.h"
#include "System/Content/CsResourceImporter.h"
#include "System/Scene/Rendering/ScnModelFileData.h"

#include "Base/BcStream.h"

//////////////////////////////////////////////////////////////////////////
// ScnModelVertexFormat
class ScnModelVertexFormat
{
	REFLECTION_DECLARE_BASIC( ScnModelVertexFormat );

	ScnModelVertexFormat(){}

	RsVertexDataType Position_ = RsVertexDataType::FLOAT32;
	RsVertexDataType Normal_ = RsVertexDataType::FLOAT32;
	RsVertexDataType Binormal_ = RsVertexDataType::FLOAT32;
	RsVertexDataType Tangent_ = RsVertexDataType::FLOAT32;
	RsVertexDataType Colour_ = RsVertexDataType::UBYTE_NORM;
	RsVertexDataType BlendIndices_ = RsVertexDataType::UBYTE;
	RsVertexDataType BlendWeights_ = RsVertexDataType::USHORT_NORM;
	RsVertexDataType TexCoord0_ = RsVertexDataType::FLOAT32;
	RsVertexDataType TexCoord1_ = RsVertexDataType::FLOAT32;
	RsVertexDataType TexCoord2_ = RsVertexDataType::FLOAT32;
	RsVertexDataType TexCoord3_ = RsVertexDataType::FLOAT32;
};

//////////////////////////////////////////////////////////////////////////
// ScnModelImport
class ScnModelImport:
	public CsResourceImporter
{
public:
	REFLECTION_DECLARE_DERIVED_MANUAL_NOINIT( ScnModelImport, CsResourceImporter );

public:
	ScnModelImport();
	ScnModelImport( ReNoInit );
	virtual ~ScnModelImport();

	/**
	 * Import.
	 */
	BcBool import( const Json::Value& Object ) override;

private:
	/**
	 * Calculate node world transforms.
	 */
	void calculateNodeWorldTransforms();

private:
	void recursiveSerialiseNodes( 
		struct aiNode* pNode,
		size_t ParentIndex,
		size_t& NodeIndex,
		size_t& PrimitiveIndex );

	void serialiseMesh( 
		struct aiMesh* Mesh,
		size_t ParentIndex,
		size_t& NodeIndex,
		size_t& PrimitiveIndex );

	void serialiseVertices( 
		struct aiMesh* Mesh,
		RsVertexElement* pVertexElements,
		size_t NoofVertexElements,
		MaAABB& AABB );

	size_t findNodeIndex( std::string Name, aiNode* RootSearchNode, size_t& BaseIndex ) const;

private:
	CsCrossRefId findMaterialMatch( const std::string& MaterialName );

private:
	std::string Source_;
	std::map< std::string, CsCrossRefId > Materials_;

	BcStream HeaderStream_;
	BcStream NodeTransformDataStream_;
	BcStream NodePropertyDataStream_;
	BcStream VertexDataStream_;
	BcStream IndexDataStream_;
	BcStream VertexElementStream_;
	BcStream MeshDataStream_;

	const struct aiScene* Scene_;

	std::vector< ScnModelNodeTransformData > NodeTransformData_;
	std::vector< ScnModelNodePropertyData > NodePropertyData_;
	std::vector< ScnModelMeshData > MeshData_;
	std::vector< RsVertexDeclarationDesc > VertexDeclarations_;
	std::vector< MaMat4d > InverseBindposes_;

	ScnModelVertexFormat VertexFormat_;
};

#endif
