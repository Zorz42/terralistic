#[derive(serde_derive::Serialize, serde_derive::Deserialize, PartialEq, Eq)]
pub enum UiMessageType {
    ServerState(ServerState),
    ConsoleMessage(String),
    MsptUpdate(u64),
    PlayerEvent(PlayerEventType),
}

#[derive(Copy, Clone, serde_derive::Serialize, serde_derive::Deserialize, PartialEq, Eq)]
pub enum ServerState {
    Nothing,
    Starting,
    InitMods,
    LoadingWorld,
    GeneratingWorld,
    Running,
    Stopping,
    Stopped,
}

#[derive(Clone, serde_derive::Serialize, serde_derive::Deserialize, PartialEq, Eq)]
pub enum PlayerEventType {
    Join((String, std::net::IpAddr)),
    Leave(std::net::IpAddr),
    //kick and ban will be added later
}