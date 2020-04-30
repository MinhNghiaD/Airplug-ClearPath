Project dependencies: 
  - Qt5::Core
  - Qt5::Network
  - Qt5::Widgets
  - Qt5::Gui
  - Qt5::Test
  - CMake version >= 3.1.0

To configure project ======> ./boostrap.sh
Go to ./build
Enter command "make"


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
                                                options : application options
                                                state   : local variables
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

