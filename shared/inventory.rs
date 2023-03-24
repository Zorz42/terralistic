use crate::shared::items::{ItemId, ItemStack};
use anyhow::{anyhow, bail, Result};
use std::collections::HashMap;

pub struct Recipe {
    pub result: ItemId,
    pub ingredients: HashMap<ItemId, i32>,
}

pub struct Recipes {
    recipes: Vec<Recipe>,
}

impl Recipes {
    #[must_use]
    pub const fn new() -> Self {
        Self {
            recipes: Vec::new(),
        }
    }

    pub fn add_recipe(&mut self, recipe: Recipe) {
        self.recipes.push(recipe);
    }

    #[must_use]
    pub const fn get_recipes(&self) -> &Vec<Recipe> {
        &self.recipes
    }
}

pub struct Inventory {
    items: Vec<Option<ItemStack>>,
}

impl Inventory {
    #[must_use]
    pub fn new(size: usize) -> Self {
        Self {
            items: vec![None; size],
        }
    }

    /// # Errors
    /// if the index is out of bounds
    pub fn get_item(&self, index: usize) -> Result<Option<&ItemStack>> {
        Ok(self
            .items
            .get(index)
            .ok_or_else(|| anyhow!("no item at index"))?
            .as_ref())
    }

    /// # Errors
    /// if the index is out of bounds
    pub fn set_item(&mut self, index: usize, item: Option<ItemStack>) -> Result<()> {
        *self
            .items
            .get_mut(index)
            .ok_or_else(|| anyhow!("Out of bounds"))? = item;
        Ok(())
    }

    #[must_use]
    pub fn get_item_count(&self, item: ItemId) -> i32 {
        let mut count = 0;
        for slot in self.items.iter().flatten() {
            if slot.item == item {
                count += slot.count;
            }
        }
        count
    }

    #[must_use]
    pub fn can_craft(&self, recipe: &Recipe) -> bool {
        for (item, count) in &recipe.ingredients {
            if self.get_item_count(*item) < *count {
                return false;
            }
        }
        true
    }

    /// # Errors
    /// if the recipe can't be crafted
    pub fn craft(&mut self, recipe: &Recipe) -> Result<()> {
        if !self.can_craft(recipe) {
            bail!("can't craft")
        }

        let mut counts_to_remove = recipe.ingredients.clone();
        for slot in &mut self.items {
            if let Some(item) = slot {
                if let Some(count) = counts_to_remove.get_mut(&item.item) {
                    if *count > 0 {
                        if item.count > *count {
                            item.count -= *count;
                            *count = 0;
                        } else {
                            *count -= item.count;
                            *slot = None;
                        }
                    }
                }
            }
        }

        let result = self
            .items
            .iter_mut()
            .find(|item| item.is_none())
            .ok_or_else(|| anyhow!("no empty slot"))?;
        *result = Some(ItemStack {
            item: recipe.result,
            count: 1,
        });
        Ok(())
    }
}
