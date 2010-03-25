/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _TILEASSEMBLER_H_
#define _TILEASSEMBLER_H_

// load our modified version first !!
#include "AABSPTree.h"

#include <G3D/Vector3.h>
#include <map>

#include "CoordModelMapping.h"
#include "SubModel.h"
#include "ModelContainer.h"

namespace VMAP
{
    enum ModelFlags
    {
        MOD_M2 = 1,
        MOD_WORLDSPAWN = 1<<1,
        MOD_HAS_BOUND = 1<<2
    };
    
    /**
    This Class is used to convert raw vector data into balanced BSP-Trees.
    To start the conversion call convertWorld().
    */
    //===============================================

    class ModelPosition
    {
        private:
            G3D::Matrix3 iRotation;
        public:
            G3D::Vector3 iPos;
            G3D::Vector3 iDir;
            float iScale;
            void init()
            {
                iRotation = G3D::Matrix3::fromEulerAnglesZYX(G3D::pi()*iDir.y/180.f, G3D::pi()*iDir.x/180.f, G3D::pi()*iDir.z/180.f);
            }
            G3D::Vector3 transform(const G3D::Vector3& pIn) const;
            void moveToBasePos(const G3D::Vector3& pBasePos) { iPos -= pBasePos; }
    };

    class ModelSpawn
    {
        public:
            //mapID, tileX, tileY, Flags, ID, Pos, Rot, Scale, Bound_lo, Bound_hi, name
            uint32 flags;
            uint32 ID;
            G3D::Vector3 iPos;
            G3D::Vector3 iRot;
            float iScale;
            G3D::AABox iBound;
            std::string name;
            bool operator==(const ModelSpawn &other) { return ID == other.ID; }
            uint32 hashCode() const { return ID; }
            // temp?
            const G3D::AABox& getAABoxBounds() const { return iBound; }

            
            static bool readFromFile(FILE *rf, ModelSpawn &spawn);
            static bool writeToFile(FILE *rw, const ModelSpawn &spawn);
    };

    typedef std::map<uint32, ModelSpawn> UniqueEntryMap;
    typedef std::multimap<uint32, uint32> TileMap;

    struct MapSpawns
    {
        UniqueEntryMap UniqueEntries;
        TileMap TileEntries;
    };

    typedef std::map<uint32, MapSpawns*> MapData;
    //===============================================

    class TileAssembler
    {
        private:
            CoordModelMapping *iCoordModelMapping;
            std::string iDestDir;
            std::string iSrcDir;
            bool (*iFilterMethod)(char *pName);
            G3D::Table<std::string, unsigned int > iUniqueNameIds;
            unsigned int iCurrentUniqueNameId;
            std::vector<WmoModelExt*> tempModelExt;
            MapData mapData;

        public:
            TileAssembler(const std::string& pSrcDirName, const std::string& pDestDirName);
            virtual ~TileAssembler();

            bool fillModelContainerArray(const std::string& pDirFileName, unsigned int pMapId, int pXPos, int pYPos, G3D::Array<ModelContainer*>& pMC);
            ModelContainer* processNames(const G3D::Array<std::string>& pPosFileNames, const char* pDestFileName);

            void init();
            bool convertWorld();
            bool convertWorld2();
            bool readMapSpawns();
            bool calculateTransformedBound(ModelSpawn &spawn);

            bool fillModelIntoTree(G3D::AABSPTree<SubModel *> *pMainTree, const G3D::Vector3& pBasePos, std::string& pPosFilename, std::string& pModelFilename);
            void getModelPosition(std::string& pPosString, ModelPosition& pModelPosition);
            bool readRawFile(std::string& pModelFilename,  ModelPosition& pModelPosition, G3D::AABSPTree<SubModel *> *pMainTree);
            void addWorldAreaMapId(unsigned int pMapId) { iCoordModelMapping->addWorldAreaMap(pMapId); }
            void setModelNameFilterMethod(bool (*pFilterMethod)(char *pName)) { iFilterMethod = pFilterMethod; }
            std::string getDirEntryNameFromModName(unsigned int pMapId, const std::string& pModPosName);
            unsigned int getUniqueNameId(const std::string pName);
            /* uint64 makeKey(uint32 tileX, uint32 tileY) { return uint64(tileX)<<32|tileY; }
            void unpackKey(uint64 key, uint32 &tileX, uint32 &tileY) { tileX = key>>32; tileY = key&0xFFFF; } */
    };
    //===============================================
}                                                           // VMAP
#endif                                                      /*_TILEASSEMBLER_H_*/
