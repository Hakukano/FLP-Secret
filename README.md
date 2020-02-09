# FLP-Secret
This is a secret ;)

# Usage
## Linux command line
### Encrypt
```bash
./secret e <in_file_path> [out_dir_path]
```
Read <in_file_path>, encrypt and write encrypted file and code to [out_dir_path] whose default is ./out/

```bash
./secret ed <in_file_dir_path> [out_dir_path]
```
Read all files in <in_file_dir_path> recursively, encrypt and write encrypted file and code to [out_dir_path] whose default is ./out/

### Decrypt
```bash
./secret d <in_file_path> <in_code_path> [out_dir_path]
```
Read <in_file_path>, decrypt using <in_code_path> and write decrypted file to [out_dir_path]

```bash
./secret dd <in_file_dir_path> <in_code_dir_path> [out_dir_path]
```
Read all files in <in_file_dir_path>, decrypt using all codes in <in_code_dir_path> and write decrypted file to [out_dir_path]

# Notice
- Do not change .<version>.flpsdata and .<version>.flpscode part, or the file can not be decrypt properly.
- Keep flpsdata and flpscode have the same file name.
- When manipulating the whole directory, make sure there are no files share a file name.

# Update plan
1. Support multiple files manipulation as well as code management.
2. Support gui.
3. Support other platfrom.
