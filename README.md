# Stateful Sun RPC
## Building
___
`make`, runs `rpcgen ssnfs.x` by default. Needed as file paths are already determined.
`make clean`, remove all objs and bin files. Along with `rpcgen` generated files.

`make server` and `make client` are just simple commands to connect to localhost.

## Using
___
The client is only able to output not input. (Not specified in instructions for input)

After the client connects the Test() function will be called. Please place the
test code there. The only change done to the wrapper function from the example
code provided are just a couple of extra parameters to add to the funcitons.

The database has a size of 16MB, contatining space for 512 files each having
a size of 64 blocks. Every block is composed of 512bytes and one of them is
reserved for file metadata. (Filename,closed_state,username,file_descriptor)

This metadata is used by the server by creating the file table if the server
crashes or restarts.
