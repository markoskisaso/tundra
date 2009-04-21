// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_Interfaces_TextureServiceInterface_h
#define incl_Interfaces_TextureServiceInterface_h

#include "ServiceInterface.h"

namespace Foundation
{    
    class TextureServiceInterface : public ServiceInterface
    {
    public:
        TextureServiceInterface() {}
        virtual ~TextureServiceInterface() {}

        //! requests a texture to be received and decoded
        /*! when texture data becomes available, an event will be sent for each quality level decoded        
            \param asset_id texture ID, UUID for legacy UDP assets            
         */
        virtual void RequestTexture(const std::string& asset_id) = 0;
    };
}

#endif
