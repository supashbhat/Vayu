#!/usr/bin/env bash

set -euo pipefail

if [[ $# -lt 3 ]]; then
  echo "Usage: $0 <au-component-path> <vst3-path> <output-pkg-path> [version]" >&2
  exit 1
fi

au_path="$1"
vst3_path="$2"
output_pkg="$3"
version="${4:-1.0.0}"

if [[ ! -d "$au_path" ]]; then
  echo "AU bundle not found: $au_path" >&2
  exit 1
fi

if [[ ! -d "$vst3_path" ]]; then
  echo "VST3 bundle not found: $vst3_path" >&2
  exit 1
fi

work_dir="$(mktemp -d)"
trap 'rm -rf "$work_dir"' EXIT

pkg_root="$work_dir/pkgroot"
mkdir -p "$pkg_root/Library/Audio/Plug-Ins/Components"
mkdir -p "$pkg_root/Library/Audio/Plug-Ins/VST3"

ditto "$au_path" "$pkg_root/Library/Audio/Plug-Ins/Components/$(basename "$au_path")"
ditto "$vst3_path" "$pkg_root/Library/Audio/Plug-Ins/VST3/$(basename "$vst3_path")"

mkdir -p "$(dirname "$output_pkg")"
pkgbuild \
  --root "$pkg_root" \
  --identifier "com.supash.vayu.plugins" \
  --version "$version" \
  --install-location "/" \
  "$output_pkg"

echo "Created installer: $output_pkg"
