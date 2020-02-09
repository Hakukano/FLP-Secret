# FLP-Secret
This is a secret ;)

# Usage
## Linux command line
### Encrypt
```bash
./secret e <in_file_path> <out_file_path> <out_code_path>
```
Read <in_file_path>, encrypt and write encrypted file to <out_file_path>, the code (key) used to encrypt to <out_code_path>
### Decrypt
```bash
./secret d <in_file_path> <in_code_path> <out_file_path>
```
Read <in_file_path>, decrypt using <in_code_path> and write decrypted file to <out_file_path>

# Notice
Keep your code carefully since this is the only way to decrypt your file.

# Update plan
1. Support multiple files manipulation as well as code management.
2. Support gui.
3. Support other platfrom.
