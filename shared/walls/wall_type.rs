use crate::shared::walls::WallId;
use rlua::UserDataMethods;

/**
Wall holds all information about a type of a wall.
 */
#[derive(Clone)]
pub struct Wall {
    pub(super) id: WallId,
    pub break_time: i32,
    pub name: String,
}

impl Default for Wall {
    fn default() -> Self {
        Self::new()
    }
}

impl Wall {
    #[must_use]
    pub const fn new() -> Self {
        Self {
            id: WallId::new(),
            break_time: 0,
            name: String::new(),
        }
    }

    #[must_use]
    pub const fn get_id(&self) -> WallId {
        self.id
    }
}

/**
make Wall Lua compatible, implement getter and setter for every field except id and image
 */
impl rlua::UserData for Wall {
    fn add_methods<'lua, M: UserDataMethods<'lua, Self>>(methods: &mut M) {
        // add meta method to set fields, id and image are not accessible
        methods.add_meta_method_mut(
            rlua::MetaMethod::NewIndex,
            |_lua_ctx, this, (key, value): (String, rlua::Value)| match value {
                rlua::Value::Integer(value) => {
                    match key.as_str() {
                        "break_time" => this.break_time = value as i32,
                        _ => {
                            return Err(rlua::Error::RuntimeError(format!(
                                "{key} is not a valid field of Wall for integer value"
                            )))
                        }
                    };
                    Ok(())
                }
                rlua::Value::String(value) => {
                    match key.as_str() {
                        "name" => this.name = value.to_str().unwrap_or("undefined").to_owned(),
                        _ => {
                            return Err(rlua::Error::RuntimeError(format!(
                                "{key} is not a valid field of Wall for string value"
                            )))
                        }
                    };
                    Ok(())
                }
                _ => Err(rlua::Error::RuntimeError(
                    "Unknown type for Wall".to_owned(),
                )),
            },
        );
    }
}