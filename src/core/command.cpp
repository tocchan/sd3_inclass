/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "command.h"
#include "core/log.h"

#include <string>
#include <map>



/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* MACROS                                                               */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* TYPES                                                                */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/
//------------------------------------------------------------------------
//------------------------------------------------------------------------
class Command
{
   public:
      Command( char const *name, command_cb cb, char const *help_text )
         : name(name)
         , callback(cb)
         , help_text(help_text)
      {}

      std::string name;
      command_cb callback;
      std::string help_text;
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
class CommandSystem
{
   public:
      //------------------------------------------------------------------------
      CommandSystem() {}
      ~CommandSystem() 
      {
         for (auto &iter : command_lookup) {
            delete iter.second;
         }

         command_lookup.clear();
      }

      //------------------------------------------------------------------------
      bool add_command( char const *name, command_cb cb, char const *help_text ) 
      {
         if (find_command(name) != nullptr) {
            // ASSERT OR DIE - DUPLICATE
            return false;
         }

         Command *command = new Command( name, cb, help_text );
         command_lookup.insert( std::pair<std::string,Command*>( name, command ) );
         return true;
      }

      //------------------------------------------------------------------------
      Command* find_command( char const *name ) const 
      {
         auto iter = command_lookup.find( name );
         if (iter != command_lookup.end()) {
            return iter->second;
         } else {
            return nullptr;
         }
      }


   public:
      std::map<std::string,Command*> command_lookup;
};

/************************************************************************/
/*                                                                      */
/* LOCAL VARIABLES                                                      */
/*                                                                      */
/************************************************************************/
static CommandSystem *gCommandSystem = nullptr;

/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL FUNCTIONS                                                      */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* EXTERNAL FUNCTIONS                                                   */
/*                                                                      */
/************************************************************************/

//------------------------------------------------------------------------
CommandSelfRegister::CommandSelfRegister( char const *id, command_cb cb, char const *help_text /*= nullptr*/ )
{
   CommandRegister( id, cb, help_text );
}

//------------------------------------------------------------------------
bool CommandRegister( char const *id, command_cb cb, char const *help_text /*= nullptr*/ )
{
   // first caller will instantiate
   if (gCommandSystem == nullptr) {
      gCommandSystem = new CommandSystem();
   }

   return gCommandSystem->add_command( id, cb, help_text );
}

//------------------------------------------------------------------------
bool CommandRun( char const *str )
{
   if (gCommandSystem == nullptr) {
      return false;
   }

   // Extract the command part from the arguments part.
   std::string line = str;
   size_t first_space = line.find( ' ' );
   std::string cmd = line.substr( 0, first_space );
   std::string args;
   if (first_space != std::string::npos) {
      args = line.substr( first_space + 1U );
   }

   // Find command.
   Command *cmdreg = gCommandSystem->find_command(cmd.c_str());
   if (nullptr == cmdreg) {
      // command did not exist
      return false;
   }

   // if we have it, run it with supplied arguments
   cmdreg->callback( args.c_str() );
   return true;
}


/************************************************************************/
/*                                                                      */
/* COMMANDS                                                             */
/*                                                                      */
/************************************************************************/
//------------------------------------------------------------------------
COMMAND( help, "Displays all commands." )
{
   if (gCommandSystem == nullptr) {
      return; // no commands?  how did this get called?!
   }

   for (auto const &iter : gCommandSystem->command_lookup) {
      Command *cmd = iter.second;

      // should print to console
      Logf( "%s: %s", cmd->name.c_str(), cmd->help_text.c_str() );
   }  
}

/************************************************************************/
/*                                                                      */
/* UNIT TESTS                                                           */
/*                                                                      */
/************************************************************************/
