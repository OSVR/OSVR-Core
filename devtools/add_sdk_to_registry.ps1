# PS Script to place in the root install prefix some code with CMake package scripts:
# it finds those package scripts and registers those directories with the per-user
# package registry for CMake. You can run it directly if you like (and know how),
# or you may use the .cmd wrapper of the same name.
#
# Created for OSVR, but may be placed and used in any suitable CMake install tree:
# nothing OSVR-specific about it.


# Copyright 2015 Sensics, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


# This function is based on a blend of
# http://blogs.msdn.com/b/luc/archive/2011/01/21/powershell-getting-the-hash-value-for-a-string.aspx
# and http://jongurgul.com/blog/get-stringhash-get-filehash/
function Hash($textToHash)
{
    $Output = New-Object System.Text.StringBuilder
    $Hasher = New-Object System.Security.Cryptography.SHA1Managed
    $Input = [System.Text.Encoding]::UTF8.GetBytes($textToHash)
    $Hasher.ComputeHash($Input) | % {$Output.Append($_.ToString("x2"))} | Out-Null
    return $Output.ToString()
}


# Registers a package in the per-user package registry
function Register-CMakePackage($PackageName, $Path)
{
    $NewFullName = (get-item $Path).FullName

    # Check that there's a package configuration script in there first.
    if (!(Test-Path (Join-Path $NewFullName "${PackageName}Config.cmake")) -and !(Test-Path (Join-Path $NewFullName "${PackageName}-config.cmake"))) {
        echo "$NewFullName : Not a valid CMake package directory"
        return
    }

    # Compute the registry location for this package.
    $RegPath = Join-Path "HKCU:\Software\Kitware\CMake\Packages" $PackageName

    if (!(Test-Path "$RegPath")) {
        echo "Making package key for $PackageName"
        New-Item -Path "$RegPath" -Force | Out-Null
    }

    # Check to see if this location is already registered for this package.
    $alreadyRegistered = $false
    $reg = Get-Item "$RegPath"
    $reg.GetValueNames() | ForEach-Object {
        $val = $reg.GetValue($_)
        if (Test-Path $val) {
            $FullName = (get-item $val).FullName
            if ($FullName -eq $NewFullName) {
                echo "$NewFullName : Already registered as a location for package '$PackageName'"
                $alreadyRegistered = $true
            }
        }
    }

    # If not already registered
    if(!($alreadyRegistered)) {
        echo "$NewFullName : Registering $PackageName installation"

        # Convert path to CMake-style
        $CMakeNewFullName = $NewFullName.Replace("\", "/")
    
        # Hash the path to use as the key
        $newKey = Hash $CMakeNewFullName

        # Create the registry value
        New-ItemProperty -Path $RegPath -Name $newKey -Value "$CMakeNewFullName" -PropertyType String -Force | Out-Null
    }
}

# Find all potential packages under this directory and try to register them.
$Root = split-path -parent $MyInvocation.MyCommand.Definition
Get-ChildItem (Join-Path $Root 'lib\cmake') | % {
    $pkg = $_.BaseName
    $path = $_.FullName
    Register-CMakePackage $pkg $path
}