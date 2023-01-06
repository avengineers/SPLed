Design of aSPL
==============

Design Constraints
------------------

The design constraints are also presumptions aSPLs design is based on.

* DesignConstraint_aspl_0001: A component name must be unique across the entire product line. Here as component name is a leaf in the tree.
* DesignConstraint_aspl_0002: The name of every element in the tree must be unique across the entire product line.
* DesignConstraint_aspl_0003: A component can have one or more variants. If there is only one kind of it, it is the default variant. (So there is no component with "no variant".)
* DesignConstraint_aspl_0004: A variant of the product line is called "a product".
* DesignConstraint_aspl_0005: There can not exist two variants of the same component inside a single product. Only zero or one variant.
  