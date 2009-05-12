
#ifndef incl_CommunicationUIModule_h
#define incl_CommunicationUIModule_h

#pragma once

#pragma warning( push )
#pragma warning( disable : 4275 ) // IConfigureCallBack used only inside CommunicationUIModule 
#include "Foundation.h"

#include "ChatSession.h"
#include "ConfigureDlg.h"
#include "DialogCallbackInterface.h"

#include "ModuleInterface.h"
#include "ContactList.h"
//#include "PythonScriptModule.h"

namespace CommunicationUI
{

	typedef boost::shared_ptr<ChatSession> ChatSessionUIPtr;
	//
	//class MODULE_API CommunicationUIModule : public Foundation::ModuleInterfaceImpl, public IConfigureCallBack
    class MODULE_API CommunicationUIModule : public Foundation::ModuleInterfaceImpl, public CommunicationUI::DialogCallBackInterface
	{
	public:
		CommunicationUIModule(void);
		virtual ~CommunicationUIModule(void);

		void Load();
		void Unload();
		void Initialize();
		void PostInitialize();
		void Uninitialize();
		void Update();

        void StartChat(const char* contact);

		virtual void Callback(std::string aConfigName, std::map<std::string, Foundation::Comms::SettingsAttribute> attributes);

		MODULE_LOGGING_FUNCTIONS
		//! returns name of this module. Needed for logging.
		static const std::string &NameStatic() { return Foundation::Module::NameFromType(type_static_); }
		static const Foundation::Module::Type type_static_ = Foundation::Module::MT_CommunicationUI;

        virtual bool HandleEvent(
            Core::event_category_id_t category_id,
            Core::event_id_t event_id, 
            Foundation::EventDataInterface* data);
		Core::event_category_id_t communication_event_category_id_;

	private:
		// event handlers
		static void testCallback(char*);
		static void connected(char*);
		static void connecting(char*);
		static void disconnected(char*);
		static void channelOpened(char*);
		static void channelClosed(char*);
		static void messagReceived(char*);
		static void contactReceived(char* t);
        static void contactStatusChanged(char*);
        static void handleAvailableStatusList(char*);

        static void contactAdded(char*);
        static void contactAddedToPublishList(char*);
        static void contactRemoved(char*);
        static void remotePending(char*);
        static void localPending(char*);

        static void incomingRequest(char*);

        

		void setOnlineStatus(char* status);
    public:
        static Foundation::ScriptObject* CallIMPyMethod(char* method, char* syntax, std::string& param);
    private:

        static std::vector<std::string> SplitString(const std::string &inString, const std::string &separator, const int &splitAmount);


	private:
		void initializeMainCommWindow();
		void SetupCommunicationServiceUsage();
		void OnAccountMenuSettings();
		void OnAccountMenuSetAccountAndPassword();
		void OnAccountMenuConnect();
		void OnAccountMenuDisconnect();
		void OnDirectChatMenuStartChat();

        void OnSetStatusOnline();
        void OnSetStatusAway();
        void OnSetStatusBusy();
        void OnSetStatusOffline();

        void OnContactListClicked();
        void OnSetPresenceMessage();

		void OnEntryDlgOk();
		void OnEntryDlgCancel();

		void OnContactAdd();
		void OnContactRemove();
        void OnRefresh();
        void OnComboChange();

        void reloadIMScript();
        void setupSciptInterface();
        void clearContactList();

        void testDialog();
		void UpdateOnlineStatusList();

        // adds contact to ui contact list
		void addContactItem(char *);

        // could be in python module as utility method (reservers memory)
        static char** buildOneStringParamArrayFromConstCharArray(const char* prm);

		Glib::RefPtr<Gnome::Glade::Xml> commUI_XML;

		// Widgets
		Gtk::Window *wndCommMain;
		Gtk::Window *dlgAccount;
		Gtk::ActionGroup *actionGroup;
		Gtk::Dialog* dlgEntry;
		//Gtk::TreeView* lstBuddies;
        Gtk::Menu* menuContactList;
        Gtk::Fixed* fxdContainer;
        ContactList lstContacts;
        Gtk::Button* btnAddContact;
        Gtk::Button* btnRemoveContact;
        Gtk::Button* btnSetPresenceMessage;
        Gtk::Entry* entryPresenceMessage;

        //Gtk::ComboBox* cmbPresence;
        Gtk::ComboBoxText cmbPresence;
        Gtk::Label* lblPresenceStatus;
        

        //class PresenceColumns : public Gtk::TreeModel::ColumnRecord
        //{
        //    public:
        //    PresenceColumns()
        //    { add(presence_); add(message_); }

        //    Gtk::TreeModelColumn<Glib::ustring> presence_;
        //    Gtk::TreeModelColumn<Glib::ustring> message_;
        //};
        //PresenceColumns presenceCols_;

        //Glib::RefPtr<Gtk::ListStore> presenceModel_;


	protected:

		int entryret_;
			
		// Currently just 1 session
		bool sessionUp_;
	public:
		ChatSessionUIPtr session_;
	    std::map<std::string, ChatSessionUIPtr> chatSessions_;

	private:
		// Service References
		boost::shared_ptr<Foundation::Comms::CommunicationManagerServiceInterface> commManager;
		Communication::CommunicationServicePtr communication_service_;
        //boost::weak_ptr<Foundation::Comms::CommunicationManagerServiceInterface> commManager;
		

		boost::shared_ptr<Foundation::ScriptServiceInterface> scriptService;
        //boost::weak_ptr<Foundation::ScriptServiceInterface> scriptService;

		// Scripts
		Foundation::ScriptObject* sobj;
		Foundation::ScriptObject* imScriptObject;

		// Pointer to ui instance (needed for accessing from static methods, which are needed for event passsing)
		static CommunicationUIModule* instance_;
		//Foundation::CommunicationUIManagerPtr CommunicationUI_manager_;

		std::map<std::string, std::string> contactList_;

	};
}
#pragma warning( pop )
#endif