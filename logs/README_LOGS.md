# Message logger (LOGS)

LOGS is a library that defines error log handling and provides a logger interface to extend log functionality

By default, the global logger throws on fatal and prints to stderr for warning and errors

# Extension

Users can implement iLogger and set it as the global logger via set_logger
