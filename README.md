# ESP32, sensors and Protocol Buffers

This project uses an ESP32 to collect data from sensors, encode it with protocol buffers and send it to a remote server.

## Protocol Buffers with Nanopb

[Nanopb](https://github.com/nanopb/nanopb) is used to generate static code to encode and decode the Protocol Buffers messages.

### Nanopb installation

Steps for Windows

- install the latest version of Protocol Buffers for windows from the [release page](https://github.com/protocolbuffers/protobuf/releases)
- Nanopb uses protobuf for python so you also need to download the release for python
- inside the folder of protobuf for python, from protobuf\python run:

  ```Powershell
  python .\setup.py build
  python .\setup.py test
  python .\setup.py install
  ```

- to generate the `.pb.c` and `.pb.h` files, from nanopb\generator run:

  ```Powershell
  python .\nanopb_generator.py protofile.proto
  ```

  > **NOTE**: These steps are only for personal reference. For official guides check [Protocol Buffers](https://developers.google.com/protocol-buffers/docs/overview) and [Nanopb](https://jpa.kapsi.fi/nanopb/docs/) documentations
