use clap::{App, Arg};
use rand::{thread_rng, Rng};
use std::{
    fs::{create_dir_all, remove_file, File},
    io::{prelude::*, BufReader, BufWriter, Error, ErrorKind, Result},
    path::Path,
};

const KEY_LEN_MIN: usize = 10;
const KEY_LEN_MAX: usize = 100;
const CHUNK_SIZE: usize = 1024;

fn add_loop<T>(origin: T, adder: T, min: T, max: T) {

}

fn generate_key() -> Vec<u8> {
    let mut rng = thread_rng();
    let len = rng.gen_range(KEY_LEN_MIN, KEY_LEN_MAX + 1);
    let mut key = Vec::with_capacity(len);
    for _ in 0..len {
        key.push(rng.gen_range(0, u8::MAX));
    }
    key
}

fn read_key(file: &str) -> Result<Vec<u8>> {
    let file = File::open(file)?;
    let metadata = file.metadata()?;
    if metadata.len() > KEY_LEN_MAX as u64 {
        return Err(Error::new(ErrorKind::Other, "Key too long"));
    }
    let mut key = Vec::with_capacity(metadata.len() as usize);
    let mut reader = BufReader::new(file);
    reader.read_to_end(&mut key)?;
    Ok(key)
}

fn main() -> Result<()> {
    let matches = App::new("FLP Secret")
        .about("A simple encrypt script")
        .version("1.0.0")
        .author("Jiaming Bao <baojiaming08@gmail.com>")
        .subcommand(
            App::new("encrypt")
                .about("To encrypt file[s]")
                .version("1.0.0")
                .author("Jiaming Bao <baojiaming08@gmail.com>")
                .arg(
                    Arg::new("key")
                        .about("Use key file. Use random key if not set.")
                        .short('k')
                        .long("key")
                        .value_name("KEY")
                        .takes_value(true),
                )
                .arg(
                    Arg::new("output")
                        .about("Output directory.")
                        .short('o')
                        .long("output")
                        .value_name("DIR")
                        .takes_value(true)
                        .default_value("./out"),
                )
                .arg(
                    Arg::new("input")
                        .about("Input file[s].")
                        .required(true)
                        .value_name("FILES")
                        .min_values(1),
                ),
        )
        .subcommand(
            App::new("decrypt")
                .about("To decrypt file[s]")
                .version("1.0.0")
                .author("Jiaming Bao <baojiaming08@gmail.com>")
                .arg(
                    Arg::new("key")
                        .about("Use key file. Use random key if not set.")
                        .short('k')
                        .long("key")
                        .value_name("KEY")
                        .takes_value(true),
                )
                .arg(
                    Arg::new("output")
                        .about("Output directory.")
                        .short('o')
                        .long("output")
                        .value_name("DIR")
                        .takes_value(true)
                        .default_value("./out"),
                )
                .arg(
                    Arg::new("input")
                        .about("Input file[s].")
                        .required(true)
                        .value_name("FILES")
                        .min_values(1),
                ),
        )
        .get_matches();

    if let Some(encrypt_matches) = matches.subcommand_matches("encrypt") {
        let key = match encrypt_matches.value_of("key") {
            None => generate_key(),
            Some(key) => read_key(key)?,
        };

        let output = encrypt_matches.value_of("output").unwrap();
        create_dir_all(output)?;

        encrypt_matches
            .values_of("input")
            .unwrap()
            .filter(|input| Path::new(input).is_file())
            .for_each(move |input| {
                let file = File::open(input).unwrap_or_else(|err| panic!(err.to_string()));
                let metadata = file
                    .metadata()
                    .unwrap_or_else(|err| panic!(err.to_string()));
                let mut file_size = metadata.len();
                let mut reader = BufReader::new(file);
                let output = Path::new(output).join(format!(
                    "{}.flpsdata",
                    Path::new(input).file_name().unwrap().to_str().unwrap()
                ));
                if output.exists() {
                    if let Err(err) = remove_file(output.clone()) {
                        panic!(err.to_string());
                    }
                }
                let output_file =
                    File::create(output).unwrap_or_else(|err| panic!(err.to_string()));
                let mut writer = BufWriter::new(output_file);
                let mut key_pos = 0;
                while file_size >= CHUNK_SIZE as u64 {
                    let mut chunk = [0; CHUNK_SIZE];
                    if let Err(err) = reader.read_exact(&mut chunk) {
                        panic!(err.to_string());
                    };
                    // TODO
                    for _ in 0..CHUNK_SIZE {

                        key_pos += 1;
                    }
                    if let Err(err) = writer.write_all(&mut chunk) {
                        panic!(err.to_string());
                    };
                    file_size -= CHUNK_SIZE as u64;
                }
            });
    }
    if let Some(decrypt_matches) = matches.subcommand_matches("decrypt") {
        let key = match decrypt_matches.value_of("key") {
            None => generate_key(),
            Some(key) => key.as_bytes().to_vec(),
        };

        let output = decrypt_matches.value_of("output").unwrap();
        create_dir_all(output)?;

        let files: Vec<_> = decrypt_matches.values_of("input").unwrap().collect();
    }

    Ok(())
}
