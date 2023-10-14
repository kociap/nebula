Contribution Guidelines
=======================
Fundamentals
------------
- Patches should not mix code changes with formatting changes.
- Contributed code must follow the Code Guidelines.
- Patches should not introduce build breaks and should be bisectable.
- Patches should be limited to the smallest scope possible, ideally only an
  individual module.

Patch message
-------------
- Lines should be limited to 75 characters so that they are properly displayed
  in 80-column mode.
- The first line should start with the name of the module and be a concise
  summary of the change written in imperative language. Example

    renderer: add batch mode rendering

- Subsequent lines should describe the patch in more detail. Example

    renderer: add batch mode rendering

    Extend the rendering API with batch mode functions that allow committing
    multiple draw commands at once. Add draw index as shader input to enable
    access to the appropriate draw data.

- If a patch references issues or MRs, include full links instead of tags or
  numbers as it makes accessing the page easier. Example:

    Closes: https://github.com/kociap/nebula/issues/1

Authors
-------
Piotr Kocia <peter.kocia@gmail.com>
