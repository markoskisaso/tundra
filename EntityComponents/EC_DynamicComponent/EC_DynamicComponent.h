// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_EC_DynamicComponent_EC_DynamicComponent_h
#define incl_EC_DynamicComponent_EC_DynamicComponent_h

#include "ComponentInterface.h"
#include "AttributeInterface.h"
#include "Declare_EC.h"

class QDomDocument;
class QDomElement;

#include <qvariant.h>

//! A 
/*! 
 */
class EC_DynamicComponent : public Foundation::ComponentInterface
{
    DECLARE_EC(EC_DynamicComponent);

    Q_OBJECT

public:
    //! Destructor.
    ~EC_DynamicComponent();

    virtual bool IsSerializable() const { return true; }

    //a test attribute - was a float to suite animation state. worked ok
    //Foundation::Attribute<float> x_;
    //! a test attribute - now a string of JSON data to make shortcut to multiple vals & types
    Foundation::Attribute<std::string> json_;

public slots:
    void AddAttribute();
    QVariant GetAttribute();
    //private slots:
    //void SetAttribute(float new_x);
    void SetAttribute(QString new_json);
    uint GetParentEntityId(); //quick hack, should use GetParentEntity in ComponentInterface and add qt things to Entity (and eventualy get rid of PyEntity)
    
private:
    //! Constuctor.
    /*! \param module Module.
     */
    explicit EC_DynamicComponent(Foundation::ModuleInterface *module);

};

#endif
