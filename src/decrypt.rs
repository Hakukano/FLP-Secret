use std::{
    fs::{remove_file, File},
    io::{prelude::*, BufReader, BufWriter, Result},
    path::Path,
};

use crate::{add_loop, sub_loop, CHUNK_SIZE};

pub fn decrypt_files(key: Vec<u8>, output: &str, input_files: Vec<&str>) -> Result<()> {
    for input in input_files {
        let file = File::open(input)?;
        let metadata = file.metadata()?;
        let mut file_size = metadata.len() as usize;
        let mut reader = BufReader::new(file);
        let output = Path::new(output).join(
            Path::new(input)
                .file_name()
                .unwrap()
                .to_str()
                .unwrap()
                .replace(".flpsdata", ""),
        );
        if output.exists() {
            remove_file(output.clone())?;
        }
        let output_file = File::create(output.clone())?;
        let mut writer = BufWriter::new(output_file);
        let mut key_pos = 0;
        while file_size >= CHUNK_SIZE {
            let mut chunk = [0; CHUNK_SIZE];
            reader.read_exact(&mut chunk)?;
            for i in 0..CHUNK_SIZE {
                chunk[i] = sub_loop(chunk[i], *key.get(key_pos).unwrap() as u8, 0, u8::MAX);
                key_pos = add_loop(key_pos, 1, 0, key.len() - 1);
            }
            writer.write_all(&mut chunk)?;
            file_size -= CHUNK_SIZE;
        }
        if file_size > 0 {
            let mut chunk = vec![0; file_size as usize];
            reader.read_exact(&mut chunk)?;
            for i in 0..file_size {
                chunk[i] = sub_loop(
                    *chunk.get(i).unwrap(),
                    *key.get(key_pos).unwrap() as u8,
                    0,
                    u8::MAX,
                );
                key_pos = add_loop(key_pos, 1, 0, key.len() - 1);
            }
            writer.write_all(&mut chunk)?;
        }
        println!("Wrote decrypted file to {}", output.to_str().unwrap());
    }
    println!("Done!");
    Ok(())
}
