Project dependencies: 
  - Qt5::Core
  - Qt5::Network
  - Qt5::Widgets
  - Qt5::Gui
  - Qt5::Test
  - CMake version >= 3.1.0

To configure project ======> ./boostrap.sh
                             Go to ./build
                             Compile source code with command "make"


Applications are built in ./build/core/apps
The scripts for network construction is in ./build/core/apps/network_scripts


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

- ACLMessage formats : {
                            performative : ACLMessage::Performative,
                            sender       : siteID
                            nbSequence   : int
                            timestamp    : VectorClock
                            content      : JsonObject
                       }

- State message format : {
                              performative : ACLMessage::INFORM_STATE,
                              sender       : siteID     (optional if send from local app)
                              nbSequence   : int        (optional if send from local app)
                              timestamp    : VectorClock
                              content      : {
                                                msgCounter : int (bilan, this field should only be attached to the first state message of each site to avoid redundance)
                                                options : application options
                                                state   : local variables
                                             }
                         }

- Prepost message format : {
                                performative : ACLMessage::PREPOST_MESSAGE,
                                sender       : siteID of receiver,
                                nbSequence   : nbSequence of receiver,
                                timestamp    : VectorClock of original message
                                content      : {
                                                    receiver : siteID of receiver of prepost message
                                                    message  : original message (include siteID and nbSequence of sender)
                                               }
                           }

- Collectted state : {
                        siteID : String
                        clock  : VectorClock
                        state  : {
                                     options : application options
                                     state   : local variables
                                 }
                     }

- Mutex request : {
                        performative : ACLMessage::REQUEST_MUTEX,
                        sender       : NET ID
                        nbSequence   : int
                        timestamp    : VectorClock of Application
                  }

- Mutex approve : {
                        performative : ACLMessage::ACCEPT_MUTEX,
                        sender       : NET ID                           (optional for local app)
                        nbSequence   : int                              (optional for local app)
                        timestamp    : VectorClock of Application       (optional for local app)
                        content      : {apps : [array of appID]}
                  }

