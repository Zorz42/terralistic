use crate::blocks::UNBREAKABLE;
use crate::walls::Walls;
use anyhow::{anyhow, Result};
use events::EventManager;

/**
Stores the info about a breaking progress about a wall.
 */
pub struct BreakingWall {
    pub break_progress: i32,
    pub is_breaking: bool,
    coord: (i32, i32),
}

impl BreakingWall {
    pub fn new() -> Self {
        BreakingWall {
            break_progress: 0,
            is_breaking: false,
            coord: (0, 0),
        }
    }

    pub fn get_coord(&self) -> (i32, i32) {
        self.coord
    }
}

impl Walls {
    /**
    Returns the break progress of the wall at x and y
     */
    pub fn get_break_progress(&self, x: i32, y: i32) -> i32 {
        for wall in &self.breaking_walls {
            if wall.coord == (x, y) {
                return wall.break_progress;
            }
        }
        0
    }

    /**
    Returns the break stage (for example to be used as a break texture stage) of the wall at x and y
     */
    pub fn get_break_stage(&self, x: i32, y: i32) -> Result<i32> {
        Ok(self.get_break_progress(x, y) * 9 / self.get_wall_type_at(x, y)?.break_time)
    }

    /**
    Includes the necessary steps to start breaking a wall, such as adding it to the breaking_walls list, setting is_breaking to true and sending the WallStartedBreakingEvent
     */
    pub fn start_breaking_wall(&mut self, x: i32, y: i32) -> Result<()> {
        if self.get_wall_type_at(x, y)?.break_time == UNBREAKABLE {
            return Ok(());
        }

        let mut breaking_wall: Option<&mut BreakingWall> = None;
        for wall in &mut self.breaking_walls {
            if wall.coord == (x, y) {
                breaking_wall = Some(wall);
                break;
            }
        }

        let breaking_wall = {
            if let Some(breaking_wall) = breaking_wall {
                breaking_wall
            } else {
                let mut new_breaking_wall = BreakingWall::new();
                new_breaking_wall.coord = (x, y);
                self.breaking_walls.push(new_breaking_wall);
                self.breaking_walls
                    .last_mut()
                    .ok_or(anyhow!("Could not get last breaking wall!"))?
            }
        };

        breaking_wall.is_breaking = true;
        Ok(())

        //self.wall_started_breaking_event.send(WallStartedBreakingEvent::new(x, y));
    }

    /**
    Includes the necessary steps to stop breaking a wall,
    such as removing it from the breaking_walls list,
    setting is_breaking to false and sending the
    WallStoppedBreakingEvent
     */
    pub fn stop_breaking_wall(&mut self, x: i32, y: i32) {
        for wall in self.breaking_walls.iter_mut() {
            if wall.coord == (x, y) {
                wall.is_breaking = false;
                //self.wall_stopped_breaking_event.send(WallStoppedBreakingEvent::new(x, y));
                break;
            }
        }
    }

    /**
    Updates breaking walls by increasing break
    progress and breaking walls if necessary
     */
    pub fn update_breaking_walls(
        &mut self, frame_length: f32, _events: &mut EventManager,
    ) -> Result<()> {
        for breaking_wall in self.breaking_walls.iter_mut() {
            if breaking_wall.is_breaking {
                breaking_wall.break_progress += frame_length as i32;
            }
        }

        let mut broken_walls = Vec::new();
        for breaking_wall in self.breaking_walls.iter() {
            if breaking_wall.break_progress
                > self
                    .get_wall_type_at(breaking_wall.get_coord().0, breaking_wall.get_coord().1)?
                    .break_time
            {
                broken_walls.push(breaking_wall.get_coord());
            }
        }

        for broken_wall in broken_walls.iter() {
            let (x, y) = *broken_wall;

            //let _event = WallBreakEvent::new(x, y);
            //self.wall_break_event.send(event);

            self.set_wall_type(x, y, self.clear)?;

            self.breaking_walls
                .retain(|breaking_wall| breaking_wall.get_coord() != *broken_wall);
        }

        Ok(())
    }
}
