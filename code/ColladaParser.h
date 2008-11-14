/** Defines the parser helper class for the collada loader */

/*
Open Asset Import Library (ASSIMP)
----------------------------------------------------------------------

Copyright (c) 2006-2008, ASSIMP Development Team
All rights reserved.

Redistribution and use of this software in source and binary forms, 
with or without modification, are permitted provided that the 
following conditions are met:

* Redistributions of source code must retain the above
copyright notice, this list of conditions and the
following disclaimer.

* Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the
following disclaimer in the documentation and/or other
materials provided with the distribution.

* Neither the name of the ASSIMP team, nor the names of its
contributors may be used to endorse or promote products
derived from this software without specific prior
written permission of the ASSIMP Development Team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

#ifndef AI_COLLADAPARSER_H_INC
#define AI_COLLADAPARSER_H_INC

#include "./irrXML/irrXMLWrapper.h"

namespace Assimp
{

/** Parser helper class for the Collada loader. Does all the XML reading and builds internal data structures from it, 
 * but leaves the resolving of all the references to the loader.
*/
class ColladaParser
{
	friend class ColladaLoader;
public:
	/** Transformation types that can be applied to a node */
	enum TransformType
	{
		TF_LOOKAT,
		TF_ROTATE,
		TF_TRANSLATE,
		TF_SCALE,
		TF_SKEW,
		TF_MATRIX
	};

	/** Contains all data for one of the different transformation types */
	struct Transform
	{
		TransformType mType;
		float f[16]; ///< Interpretation of data depends on the type of the transformation 
	};

	/** A node in a scene hierarchy */
	struct Node
	{
		std::string mName;
		std::string mID;
		Node* mParent;
		std::vector<Node*> mChildren;

		/** Operations in order to calculate the resulting transformation to parent. */
		std::vector<Transform> mTransforms;

		Node() { mParent = NULL; }
		~Node() { for( std::vector<Node*>::iterator it = mChildren.begin(); it != mChildren.end(); ++it) delete *it; }
	};

	/** Data source array */
	struct Data
	{
		std::vector<float> mValues;
	};

	/** Accessor to a data array */
	struct Accessor
	{
		unsigned int mCount;   // in number of objects
		unsigned int mOffset;  // in number of values
		unsigned int mStride;  // Stride in number of values
		std::string mSource;   // URL of the source array
	};

	/** Contains data for a single mesh */
	struct Mesh
	{
		
	};

protected:
	/** Constructor from XML file */
	ColladaParser( const std::string& pFile);

	/** Destructor */
	~ColladaParser();

	/** Reads the contents of the file */
	void ReadContents();

	/** Reads the structure of the file */
	void ReadStructure();

	/** Reads asset informations such as coordinate system informations and legal blah */
	void ReadAssetInfo();

	/** Reads the geometry library contents */
	void ReadGeometryLibrary();

	/** Reads a mesh from the geometry library */
	void ReadGeometry();

	/** Reads the library of node hierarchies and scene parts */
	void ReadSceneLibrary();

	/** Reads a scene node's contents including children and stores it in the given node */
	void ReadSceneNode( Node* pNode);

	/** Reads a node transformation entry of the given type and adds it to the given node's transformation list. */
	void ReadNodeTransformation( Node* pNode, TransformType pType);

	/** Reads the collada scene */
	void ReadScene();

protected:
	/** Aborts the file reading with an exception */
	void ThrowException( const std::string& pError) const;

	/** Skips all data until the end node of the current element */
	void SkipElement();

	/** Compares the current xml element name to the given string and returns true if equal */
	bool IsElement( const char* pName) const { assert( mReader->getNodeType() == irr::io::EXN_ELEMENT); return strcmp( mReader->getNodeName(), pName) == 0; }

	/** Tests for the closing tag of the given element, throws an exception if not found */
	void TestClosing( const char* pName);

	/** Checks the present element for the presence of the attribute, returns its index or throws an exception if not found */
	int GetAttribute( const char* pAttr) const;

	/** Returns the index of the named attribute or -1 if not found. Does not throw, therefore useful for optional attributes */
	int TestAttribute( const char* pAttr) const;

	/** Reads the text contents of an element, throws an exception if not given. Skips leading whitespace. */
	const char* GetTextContent();

	/** Calculates the resulting transformation fromm all the given transform steps */
	aiMatrix4x4 CalculateResultTransform( const std::vector<Transform>& pTransforms) const;

protected:
	/** Filename, for a verbose error message */
	std::string mFileName;

	/** XML reader */
	irr::io::IrrXMLReader* mReader;

	/** node library: root node of the hierarchy part by ID */
	typedef std::map<std::string, Node*> NodeLibrary;
	NodeLibrary mNodeLibrary;

	/** Pointer to the root node. Don't delete, it just points to one of the nodes in the node library. */
	Node* mRootNode;

	/** Size unit: how large compared to a meter */
	float mUnitSize;

	/** Which is the up vector */
	enum { UP_X, UP_Y, UP_Z } mUpDirection;
};

} // end of namespace Assimp

#endif // AI_COLLADAPARSER_H_INC