// Tests for the events library.
// It tests the event manager and the event struct.

#[allow(unused_imports)]
// these two imports are not needed, but the ide complains if they are not here
use crate::libraries::events::{Event, EventManager};

#[test]
#[allow(clippy::unwrap_used)]
fn test_event_manager() {
    let mut event_manager = EventManager::new();
    let event = Event::new(5);
    event_manager.push_event(event);
    let event = event_manager.pop_event().unwrap();
    assert_eq!(event.downcast::<i32>().unwrap(), &5);
}

// test event manager with multiple events
#[test]
#[allow(clippy::unwrap_used)]
fn test_event_manager_multiple() {
    let mut event_manager = EventManager::new();
    let event = Event::new(5);
    event_manager.push_event(event);
    let event = Event::new(6);
    event_manager.push_event(event);
    let event = Event::new(7);
    event_manager.push_event(event);
    let event = event_manager.pop_event().unwrap();
    assert_eq!(event.downcast::<i32>().unwrap(), &5);
    let event = event_manager.pop_event().unwrap();
    assert_eq!(event.downcast::<i32>().unwrap(), &6);
    let event = Event::new(8);
    event_manager.push_event(event);
    let event = event_manager.pop_event().unwrap();
    assert_eq!(event.downcast::<i32>().unwrap(), &7);
    let event = event_manager.pop_event().unwrap();
    assert_eq!(event.downcast::<i32>().unwrap(), &8);
}

// test that event manager returns none when there are no events
#[test]
#[allow(clippy::unwrap_used)]
fn test_event_manager_none() {
    let mut event_manager = EventManager::new();
    assert!(event_manager.pop_event().is_none());
    let event = Event::new(5);
    event_manager.push_event(event);
    event_manager.pop_event().unwrap();
    assert!(event_manager.pop_event().is_none());
}

// test event manager default
#[test]
#[allow(clippy::unwrap_used)]
fn test_event_manager_default() {
    let mut event_manager = EventManager::default();
    let event = Event::new(5);
    event_manager.push_event(event);
    let event = event_manager.pop_event().unwrap();
    assert_eq!(event.downcast::<i32>().unwrap(), &5);
}

#[test]
#[allow(clippy::unwrap_used)]
fn test_event() {
    let event = Event::new(5);
    assert_eq!(event.downcast::<i32>().unwrap(), &5);
}

#[test]
fn test_event_fail() {
    let event = Event::new(5);
    assert!(event.downcast::<String>().is_none());
}