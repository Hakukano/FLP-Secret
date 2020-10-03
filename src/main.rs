use clap::{App, Arg};
use num_traits::{NumCast, ToPrimitive};
use rand::{thread_rng, Rng};
use std::{
    fs::{create_dir_all, File},
    io::{prelude::*, BufReader, Error, ErrorKind, Result},
    path::Path,
};

pub mod decrypt;
pub mod encrypt;

pub const CHUNK_SIZE: usize = 1024;

const VERSION: &str = "2.0.0";

const KEY_LEN_MIN: usize = 1000;
const KEY_LEN_MAX: usize = 2000;

pub fn add_loop<T>(origin: T, adder: T, min: T, max: T) -> T
where
    T: Ord + Copy + ToPrimitive + NumCast,
{
    let origin: u64 = NumCast::from(origin).unwrap();
    let adder: u64 = NumCast::from(adder).unwrap();
    let min: u64 = NumCast::from(min).unwrap();
    let max: u64 = NumCast::from(max).unwrap();

    let base = max - min + 1;
    let adder = adder % base;
    if origin > max - adder {
        return NumCast::from(adder - (base - origin)).unwrap();
    }
    NumCast::from(origin + adder).unwrap()
}

pub fn sub_loop<T>(origin: T, suber: T, min: T, max: T) -> T
where
    T: Ord + Copy + ToPrimitive + NumCast,
{
    let origin: u64 = NumCast::from(origin).unwrap();
    let suber: u64 = NumCast::from(suber).unwrap();
    let min: u64 = NumCast::from(min).unwrap();
    let max: u64 = NumCast::from(max).unwrap();

    let base = max - min + 1;
    let suber = suber % base;
    if origin < min + suber {
        return NumCast::from(base - (suber - (origin - min)) + min).unwrap();
    }
    NumCast::from(origin - suber).unwrap()
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
        .version(VERSION)
        .author("Jiaming Bao <baojiaming08@gmail.com>")
        .subcommand(
            App::new("encrypt")
                .about("To encrypt file[s]")
                .version(VERSION)
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
                .version(VERSION)
                .author("Jiaming Bao <baojiaming08@gmail.com>")
                .arg(
                    Arg::new("key")
                        .about("Key file for decryption.")
                        .required(true)
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

        let input_files: Vec<_> = encrypt_matches
            .values_of("input")
            .unwrap()
            .filter(|input| Path::new(input).is_file())
            .collect();

        encrypt::encrypt_files(key, output, input_files)?;
    }
    if let Some(decrypt_matches) = matches.subcommand_matches("decrypt") {
        let key = read_key(decrypt_matches.value_of("key").unwrap())?;

        let output = decrypt_matches.value_of("output").unwrap();
        create_dir_all(output)?;

        let input_files: Vec<_> = decrypt_matches
            .values_of("input")
            .unwrap()
            .filter(|input| Path::new(input).is_file())
            .collect();

        decrypt::decrypt_files(key, output, input_files)?;
    }

    Ok(())
}
