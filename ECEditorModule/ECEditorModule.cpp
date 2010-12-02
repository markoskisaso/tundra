// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "ECEditorModule.h"
#include "ECEditorWindow.h"
#include "EcXmlEditorWidget.h"
#include "DoxygenDocReader.h"
#include "TreeWidgetItemExpandMemory.h"

#include "EventManager.h"
#include "SceneEvents.h"
#include "NetworkEvents.h"
#include "SceneManager.h"
#include "ConsoleCommandServiceInterface.h"
#include "ModuleManager.h"
#include "EC_DynamicComponent.h"
#include "UiServiceInterface.h"
#include "Input.h"
#include "NaaliUi.h"
#include "NaaliMainWindow.h"

#include "MemoryLeakCheck.h"

#include <QWebView>

namespace ECEditor
{
    std::string ECEditorModule::name_static_ = "ECEditor";
    
    ECEditorModule::ECEditorModule() :
        IModule(name_static_),
        scene_event_category_(0),
        network_state_event_category_(0),
        xmlEditor_(0),
        active_editor_(0)
    {
    }
    
    ECEditorModule::~ECEditorModule()
    {
    }
    
    void ECEditorModule::Load()
    {
    }

    void ECEditorModule::Initialize()
    {
        expandMemory = ExpandMemoryPtr(new TreeWidgetItemExpandMemory(name_static_.c_str(), framework_->GetDefaultConfig()));
        expandMemory = ExpandMemoryPtr(new TreeWidgetItemExpandMemory(name_static_.c_str(), framework_->GetDefaultConfig()));
    }

    void ECEditorModule::PostInitialize()
    {
        /*RegisterConsoleCommand(Console::CreateCommand("ECEditor",
            "Shows the EC editor.",
            Console::Bind(this, &ECEditorModule::ShowWindow)));*/

        RegisterConsoleCommand(Console::CreateCommand("EditDynComp",
            "Command that will create/remove components from the dynamic component."
            "Params:"
            " 0 = entity id."
            " 1 = operation (add or rem)"
            " 2 = component type.(ec. EC_DynamicComponent)"
            " 3 = attribute name."
            " 4 = attribute type. (Add only)"
            " 5 = attribute value. (Add only)",
            Console::Bind(this, &ECEditorModule::EditDynamicComponent)));

        RegisterConsoleCommand(Console::CreateCommand("ShowDocumentation",
            "Prints the class documentation for the given symbol."
            "Params:"
            " 0 = The symbol to fetch the documentation for.",
            Console::Bind(this, &ECEditorModule::ShowDocumentation)));

        scene_event_category_ = framework_->GetEventManager()->QueryEventCategory("Scene");
        network_state_event_category_ = framework_->GetEventManager()->QueryEventCategory("NetworkState");

        AddEditorWindowToUI();

        inputContext = framework_->GetInput()->RegisterInputContext("ECEditorInput", 90);
        connect(inputContext.get(), SIGNAL(KeyPressed(KeyEvent *)), this, SLOT(HandleKeyPressed(KeyEvent *)));
    }

    void ECEditorModule::Uninitialize()
    {
        /// @todo make sure that this wont cause any problems in else where.
        SAFE_DELETE_LATER(active_editor_);
        SAFE_DELETE_LATER(xmlEditor_);
    }

    void ECEditorModule::Update(f64 frametime)
    {
        RESETPROFILER;
    }

    bool ECEditorModule::HandleEvent(event_category_id_t category_id, event_id_t event_id, IEventData* data)
    {
        /*if (category_id == scene_event_category_)
        {            switch(event_id)
            {
            case Scene::Events::EVENT_ENTITY_CLICKED:
            {
                //! \todo support multiple entity selection
                Scene::Events::EntityClickedData *entity_clicked_data = dynamic_cast<Scene::Events::EntityClickedData *>(data);
                if (editor_window_ && entity_clicked_data)
                    editor_window_->AddEntity(entity_clicked_data->entity->GetId());
                break;
            }
            case Scene::Events::EVENT_ENTITY_SELECT:
                //if (editor_window_)
                //    editor_window_->AddEntity(entity_clicked_data->entity->GetId());
                break;
            case Scene::Events::EVENT_ENTITY_DESELECT:
                //if (editor_window_)
                //    editor_window_->RemoveEntity(entity_clicked_data->entity->GetId());
                break;
            case Scene::Events::EVENT_ENTITY_DELETED:
            {
                Scene::Events::SceneEventData *entity_clicked_data = dynamic_cast<Scene::Events::SceneEventData*>(data);
                if (editor_window_ && entity_clicked_data)
                    editor_window_->RemoveEntity(entity_clicked_data->localID);
                break;
            }
            default:
                break;
            }
        }*/

        if (category_id == network_state_event_category_ && event_id == ProtocolUtilities::Events::EVENT_SERVER_DISCONNECTED)
            if (active_editor_)
                active_editor_->ClearEntities(); 

        return false;
    }

    ECEditorWindow *ECEditorModule::GetActiveECEditor() const
    {
        return active_editor_;
    }

    /*void ECEditorModule::RegisterECEditor(ECEditorWindow *editor) 
    {
        if (!editor)
            return;

        if (!editors_.contains(editor))
        {
            for(uint i = 0; i < editors_.size(); ++i)
                if (editors_[i])
                {
                    connect(editor, SIGNAL(OnFocusChanged(ECEditorWindow*)), editors_[i], SLOT(FocusChanged(ECEditorWindow *)), Qt::UniqueConnection);
                    connect(editors_[i], SIGNAL(OnFocusChanged(ECEditorWindow*)), editor, SLOT(FocusChanged(ECEditorWindow *)), Qt::UniqueConnection);
                }
            connect(editor, SIGNAL(destroyed()), this, SLOT(UnregisterECEditor()));
            editors_.push_back(editor);
        }
    }

    void ECEditorModule::UnregisterECEditor()
    {
        ECEditorWindow *editor = qobject_cast<ECEditorWindow*>(sender());
        if (editor)
        {
            for(ECEditorWindowList::iterator iter = editors_.begin();
                iter != editors_.end();
                ++iter)
            {
                if ((*iter) == editor)
                {
                    editors_.erase(iter);
                    break;
                }
            }
        }
    }*/
    void ECEditorModule::ECEditorFocusChanged(ECEditorWindow *editor)
    {
        if (editor == active_editor_ && !editor)
            return;

        // Unfocus previously active editor.
        if (active_editor_)
        {
            active_editor_->SetFocus(false);
            disconnect(active_editor_, SIGNAL(destroyed(QObject*)), this, SLOT(ActiveECEditorDestroyed(QObject*)));
        }
        active_editor_ = editor;
        active_editor_->SetFocus(true);
        connect(active_editor_, SIGNAL(destroyed(QObject*)), SLOT(ActiveECEditorDestroyed(QObject*)), Qt::UniqueConnection);
    }

    void ECEditorModule::AddEditorWindowToUI()
    {
        if (active_editor_)
        {
            //editor_window_->setVisible(!(editor_window_->isVisible()));
            active_editor_->setVisible(!active_editor_->isVisible());
            return;
        }

        //UiServiceInterface *ui = framework_->GetService<UiServiceInterface>(); 
        //if (!ui)
        //    return;
        NaaliUi *ui = GetFramework()->Ui();
        if (!ui)
            return;

        active_editor_ = new ECEditorWindow(GetFramework());
        active_editor_->setParent(ui->MainWindow());
        active_editor_->setWindowFlags(Qt::Tool);
        active_editor_->setAttribute(Qt::WA_DeleteOnClose);

        //UiProxyWidget *editor_proxy = ui->AddWidgetToScene(editor_window_);
        // We need to listen proxy widget's focus signal, because for some reason QWidget's focusInEvent wont get triggered when
        // it's attached to QGraphicsProxyWidget.
        //connect(editor_proxy, SIGNAL(FocusChanged(QFocusEvent *)), editor_window_, SLOT(FocusChanged(QFocusEvent *)), Qt::UniqueConnection);

        // We don't need to worry about attaching ECEditorWindow to ui scene, because ECEditorWindow's initialize operation will do it automaticly.
        //ui->AddWidgetToMenu(editor_window_, tr("Entity-component Editor"), "", "./data/ui/images/menus/edbutton_OBJED_normal.png");
        //ui->RegisterUniversalWidget("Components", editor_window_->graphicsProxyWidget());
    }

    /*Console::CommandResult ECEditorModule::ShowWindow(const StringVector &params)
    {
        UiServicePtr ui = framework_->GetService<UiServiceInterface>(Service::ST_Gui).lock();
        if (!ui)
            return Console::ResultFailure("Failed to acquire UiModule pointer!");

        if (editor_window_)
        {
            ui->BringWidgetToFront(editor_window_);
            return Console::ResultSuccess();
        }
        else
            return Console::ResultFailure("EC Editor window was not initialised, something went wrong on startup!");
    }*/

    Console::CommandResult ECEditorModule::ShowDocumentation(const StringVector &params)
    {
        if (params.size() == 0)
            return Console::ResultFailure("The first parameter must be the documentation symbol to find!");

        QUrl styleSheetPath;
        QString documentation;
        /*bool success = */DoxygenDocReader::GetSymbolDocumentation(params[0].c_str(), &documentation, &styleSheetPath);
        if (documentation.length() == 0)
            return Console::ResultFailure("Failed to find documentation!");

        QWebView *webview = new QWebView();
        webview->setHtml(documentation, styleSheetPath);
        webview->show();
        webview->setAttribute(Qt::WA_DeleteOnClose);

        return Console::ResultSuccess();
    }

    /* Params
     * 0 = entity id.
     * 1 = operation (add/rem)
     * 2 = component type.
     * 3 = attribute name
     * 4 = attribute type
     * 5 = attribute value
     */
    Console::CommandResult ECEditorModule::EditDynamicComponent(const StringVector &params)
    {
        Scene::SceneManager *sceneMgr = framework_->GetDefaultWorldScene().get();
        if(!sceneMgr)
            return Console::ResultFailure("Failed to find main scene.");

        if(params.size() == 6)
        {
            entity_id_t id = ParseString<entity_id_t>(params[0]);
            Scene::Entity *ent = sceneMgr->GetEntity(id).get();
            if(!ent)
                return Console::ResultFailure("Cannot find entity by name of " + params[0]);

            if(params[1] == "add")
            {
                ComponentPtr comp = ent->GetComponent(QString::fromStdString(params[2]));
                EC_DynamicComponent *dynComp = dynamic_cast<EC_DynamicComponent *>(comp.get());
                if(!dynComp)
                    return Console::ResultFailure("Invalid component type " + params[2]);
                IAttribute *attribute = dynComp->CreateAttribute(QString::fromStdString(params[4]), params[3].c_str());
                if(!attribute)
                    return Console::ResultFailure("Failed to create attribute type " + params[4]);
                attribute->FromString(params[5], AttributeChange::Default);
                //dynComp->ComponentChanged("Default");//AttributeChange::Local); 
            }
        }
        if(params.size() == 4)
        {
            entity_id_t id = ParseString<entity_id_t>(params[0]);
            Scene::Entity *ent = sceneMgr->GetEntity(id).get();
            if(!ent)
                return Console::ResultFailure("Cannot find entity by name of " + params[0]);

            else if(params[1] == "rem")
            {
                ComponentPtr comp = ent->GetComponent(QString::fromStdString(params[2]));
                EC_DynamicComponent *dynComp = dynamic_cast<EC_DynamicComponent *>(comp.get());
                if(!dynComp)
                    return Console::ResultFailure("Wrong component typename " + params[2]);
                dynComp->RemoveAttribute(QString::fromStdString(params[3]));
                dynComp->ComponentChanged(AttributeChange::Default);
            }
        }
        return Console::ResultSuccess();
    }

    void ECEditorModule::CreateXmlEditor(Scene::EntityPtr entity)
    {
        QList<Scene::EntityPtr> entities;
        entities << entity;
        CreateXmlEditor(entities);
    }

    void ECEditorModule::CreateXmlEditor(const QList<Scene::EntityPtr> &entities)
    {
        UiServicePtr ui = framework_->GetService<UiServiceInterface>(Service::ST_Gui).lock();
        if (entities.empty() || !ui)
            return;

        if (!xmlEditor_)
        {
            xmlEditor_ = new EcXmlEditorWidget(framework_);
            ui->AddWidgetToScene(xmlEditor_);
        }

        xmlEditor_->SetEntity(entities);
        ui->BringWidgetToFront(xmlEditor_);
    }

    void ECEditorModule::CreateXmlEditor(ComponentPtr component)
    {
        QList<ComponentPtr> components;
        components << component;
        CreateXmlEditor(components);
    }

    void ECEditorModule::CreateXmlEditor(const QList<ComponentPtr> &components)
    {
        UiServicePtr ui = framework_->GetService<UiServiceInterface>(Service::ST_Gui).lock();
        if (components.empty() || !ui)
            return;

        if (!xmlEditor_)
        {
            xmlEditor_ = new EcXmlEditorWidget(framework_);
            ui->AddWidgetToScene(xmlEditor_);
        }

        xmlEditor_->SetComponent(components);
        ui->BringWidgetToFront(xmlEditor_);
    }

    void ECEditorModule::HandleKeyPressed(KeyEvent *e)
    {
        if (e->eventType != KeyEvent::KeyPressed || e->keyPressCount > 1)
            return;

        Input &input = *framework_->GetInput();

        const QKeySequence showEcEditor = input.KeyBinding("ShowECEditor", QKeySequence(Qt::ShiftModifier + Qt::Key_E));
        if (QKeySequence(e->keyCode | e->modifiers) == showEcEditor)
        {
            if (!active_editor_)
                AddEditorWindowToUI();
            active_editor_->show();
        }
            //ShowWindow(StringVector());  
    }

    void ECEditorModule::ActiveECEditorDestroyed(QObject *obj)
    {
        if (active_editor_ == obj)
            active_editor_ = 0;
    }

}

extern "C" void POCO_LIBRARY_API SetProfiler(Foundation::Profiler *profiler);
void SetProfiler(Foundation::Profiler *profiler)
{
    Foundation::ProfilerSection::SetProfiler(profiler);
}

using namespace ECEditor;

POCO_BEGIN_MANIFEST(IModule)
    POCO_EXPORT_CLASS(ECEditorModule)
POCO_END_MANIFEST

