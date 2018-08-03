GridFS file
===========

Methods
-------

### file:getAliases()
Returns the aliases specified for `file` or `nil` if not set.

### file:getChunkSize()
Returns the chunk size specified for `file`.

### file:getContentType()
Returns the content type specified for `file` or `nil` if not set.

### file:getFilename()
Returns the filename specified for `file` or `nil` if not set.

### file:getId()
Returns the ID specified for `file`.

### file:getLength()
Returns the length of `file` in bytes.

### file:getMD5()
Returns the MD5 specified for `file` or `nil` if not set.

### file:getMetadata()
Returns the metadata specified for `file` or `nil` if not set.

### file:getUploadDate()
Returns the upload date in milliseconds since the UNIX epoch specified for `file`.

### file:read(length)
Reads up to `length` bytes from `file` and returns a string or `nil` if end-of-file was encountered.
On error, returns `nil` and the error message.

### file:remove()
Removes `file` and returns `true`. On error, returns `nil` and the error message.

### file:save()
Saves modifications to `file` to the server and returns `true`. On error, returns `nil` and the
error message.

### file:seek(offset, [whence])
Sets the file position indicator for `file` to `offset` relative to `whence` (a string) that can be
one of the following:
- `set`: beginning of file (default);
- `cur`: current position;
- `end`: end of file;

Returns `true` on success or `false` on error.

### file:setAliases(value)
Sets the aliases for `file`. A call to `file:save()` saves the change.

### file:setContentType(value)
Sets the content type for `file`. A call to `file:save()` saves the change.

### file:setFilename(value)
Sets the filename for `file`. A call to `file:save()` saves the change.

### file:setId(value)
Sets the ID for `file`. A call to `file:save()` saves the change.

### file:setMD5(value)
Sets the MD5 for `file`. A call to `file:save()` saves the change.

### file:setMetadata(value)
Sets the metadata for `file`. A call to `file:save()` saves the change.

### file:tell()
Returns the current file position indicator for `file`.

### file:write(data)
Writes `data` to `file` and returns the number of bytes written. On error, returns `nil` and the
error message.


Operators
---------

### #file
Returns the length of `file` in bytes. Synonym for `file:getLength()`.
