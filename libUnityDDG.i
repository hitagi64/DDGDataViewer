%module libUnityDDG
%{
#include "DDG/DDGCommon.h"
#include "DDG/DDGContent.h"
#include "DDG/DDGDat.h"
#include "DDG/DDGMapModelLUT.h"
#include "DDG/DDGMemoryBuffer.h"
#include "DDG/DDGPdb.h"
#include "DDG/DDGTrack.h"
#include "DDG/DDGTrackPoints.h"
#include "DDG/DDGTxm.h"
#include "DDG/DDGWorldPoints.h"

class DDGTypeCast{
public:
    static DDGDat *castToDDGDat(DDGContent *obj)
    {
        return (DDGDat*)obj;
    }
    
    static DDGMapModelLUT *castToDDGMapModelLUT(DDGContent *obj)
    {
        return (DDGMapModelLUT*)obj;
    }
    
    static DDGPdb *castToDDGPdb(DDGContent *obj)
    {
        return (DDGPdb*)obj;
    }
    
    static DDGTrack *castToDDGTrack(DDGContent *obj)
    {
        return (DDGTrack*)obj;
    }
    
    static DDGTrackPoints *castToDDGTrackPoints(DDGContent *obj)
    {
        return (DDGTrackPoints*)obj;
    }
    
    static DDGTxm *castToDDGTxm(DDGContent *obj)
    {
        return (DDGTxm*)obj;
    }
    
    static DDGWorldPoints *castToDDGWorldPoints(DDGContent *obj)
    {
        return (DDGWorldPoints*)obj;
    }
};
%}

%include "stdint.i"
%include "std_vector.i"
%include "std_string.i"
%include <std_shared_ptr.i>

%shared_ptr(DDGContent);
%shared_ptr(DDGDat);
%shared_ptr(DDGMapModelLUT);
%shared_ptr(DDGPdb);
%shared_ptr(DDGTrack);
%shared_ptr(DDGTrackPoints);
%shared_ptr(DDGTxm);
%shared_ptr(DDGWorldPoints);

%template(VectorDDGContentShared) std::vector<std::shared_ptr<DDGContent>>;
%template(VectorInt16_t) std::vector<int16_t>;
%template(VectorUint8_t) std::vector<uint8_t>;
%template(VectorDDGVector4) std::vector<DDGVector4>;
%template(VectorDDGVector3) std::vector<DDGVector3>;
%template(VectorDDGModelTexture) std::vector<DDGModelTexture>;
%template(VectorDDGVector2) std::vector<DDGVector2>;
%template(VectorDDGVertexSegment) std::vector<DDGVertexSegment>;
%template(VectorFloat) std::vector<float>;
%template(VectorDDGTrackPiece) std::vector<DDGTrackPiece>;
%template(VectorDDGTrackPoint) std::vector<DDGTrackPoint>;
%template(VectorDDGWorldPoint) std::vector<DDGWorldPoint>;

%include "DDG/DDGCommon.h"
%include "DDG/DDGContent.h"
%include "DDG/DDGDat.h"
%include "DDG/DDGMapModelLUT.h"
%include "DDG/DDGMemoryBuffer.h"
%include "DDG/DDGPdb.h"
%include "DDG/DDGTrack.h"
%include "DDG/DDGTrackPoints.h"
%include "DDG/DDGTxm.h"
%include "DDG/DDGWorldPoints.h"

class DDGTypeCast{
public:
    static DDGDat *castToDDGDat(DDGContent *obj)
    {
        return (DDGDat*)obj;
    }
    
    static DDGMapModelLUT *castToDDGMapModelLUT(DDGContent *obj)
    {
        return (DDGMapModelLUT*)obj;
    }
    
    static DDGPdb *castToDDGPdb(DDGContent *obj)
    {
        return (DDGPdb*)obj;
    }
    
    static DDGTrack *castToDDGTrack(DDGContent *obj)
    {
        return (DDGTrack*)obj;
    }
    
    static DDGTrackPoints *castToDDGTrackPoints(DDGContent *obj)
    {
        return (DDGTrackPoints*)obj;
    }
    
    static DDGTxm *castToDDGTxm(DDGContent *obj)
    {
        return (DDGTxm*)obj;
    }
    
    static DDGWorldPoints *castToDDGWorldPoints(DDGContent *obj)
    {
        return (DDGWorldPoints*)obj;
    }
};