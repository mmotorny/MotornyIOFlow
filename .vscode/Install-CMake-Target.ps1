# Copyright (c) 2021, Maksym Motorny. All rights reserved.
# Use of this source code is governed by a 3-clause BSD license that can be
# found in the LICENSE file.

Param([string] $Build, [string] $Target, [string] $Config)

$PathToXPlane11 = cat $Env:LOCALAPPDATA\x-plane_install_11.txt
cmake --install $Build --prefix $PathToXPlane11\Resources\plugins\$Target --config $Config --component $Config
