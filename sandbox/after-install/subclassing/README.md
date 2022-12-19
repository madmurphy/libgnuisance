Subclassing GNUIsance
=====================

In this directory are some examples on how to subclass parts of
**libgnuisance**.

Currently only `GnuiFlow` and `GnuiFlowLayout` are derivable. If you need to
create your own custom widget and this needs to allocate its children like
`GnuiFlow` does, you do not necessarily need to derive it from `GnuiFlow`. In
fact `GnuiFlowLayout` is reusable, and any widget that adopts it will behave
like `GnuiFlow`.

The rule of thumb is the following:

* If you need to _extend_ the properties and methods offered by `GnuiFlow` and
  create more functionalities, derive you widget from `GnuiFlow` (please refer
  to `sandbox/subclassing/gnuiflow` for an example)
* If you need instead to _get rid_ of all the properties and methods offered by
  `GnuiFlow`, derive your widget from `GtkWidget` and use `GnuiFlowLayout` as
  layout manager (please refer to
  `sandbox/subclassing/gtkwidget-with-gnuiflowlayout` for an
  example).

To the second group of widgets belongs `GnuiTaggedEntry`, which allocates its
children only internally and is derived from `GtkWidget` but uses
`GnuiFlowLayout` as layout manager.

Happy hacking :-)
