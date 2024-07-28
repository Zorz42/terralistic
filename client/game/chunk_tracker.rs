use anyhow::{anyhow, Result};
use std::collections::BTreeSet;

/// Tracks chunks and their modification time
/// allows you to get the earliest modified chunk
/// used to delete unused chunks to save memory
pub struct ChunkTracker {
    modified_time: Vec<u32>,
    timer: std::time::Instant,
    queue: BTreeSet<(u32, usize)>,
}

impl ChunkTracker {
    pub fn new(size: usize) -> Self {
        Self {
            modified_time: vec![0; size],
            timer: std::time::Instant::now(),
            queue: BTreeSet::new(),
        }
    }

    fn get_modified_time(&mut self, chunk: usize) -> Result<&mut u32> {
        self.modified_time.get_mut(chunk).ok_or_else(|| anyhow!("Chunk out of bounds"))
    }

    pub fn update(&mut self, chunk: usize) -> Result<()> {
        let time = self.timer.elapsed().as_secs() as u32;
        if *self.get_modified_time(chunk)? != 0 {
            self.remove_chunk(chunk)?;
        }
        *self.get_modified_time(chunk)? = time;
        self.queue.insert((time, chunk));
        Ok(())
    }

    pub fn get_oldest_chunk(&self) -> Result<usize> {
        self.queue.first().ok_or_else(|| anyhow!("No chunks in queue")).map(|&(_, chunk)| chunk)
    }

    pub fn get_num_chunks(&self) -> usize {
        self.queue.len()
    }

    pub fn remove_chunk(&mut self, chunk: usize) -> Result<()> {
        let time = *self.get_modified_time(chunk)?;
        self.queue.remove(&(time, chunk));
        *self.get_modified_time(chunk)? = 0;
        Ok(())
    }
}
