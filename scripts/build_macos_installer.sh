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

au_pkg="$work_dir/Vayu-AU.pkg"
vst3_pkg="$work_dir/Vayu-VST3.pkg"
distribution="$work_dir/Distribution.xml"

pkgbuild \
  --component "$au_path" \
  --identifier "com.supash.vayu.au" \
  --version "$version" \
  --install-location "/Library/Audio/Plug-Ins/Components" \
  "$au_pkg"

pkgbuild \
  --component "$vst3_path" \
  --identifier "com.supash.vayu.vst3" \
  --version "$version" \
  --install-location "/Library/Audio/Plug-Ins/VST3" \
  "$vst3_pkg"

productbuild \
  --synthesize \
  --package "$au_pkg" \
  --package "$vst3_pkg" \
  "$distribution"

mkdir -p "$(dirname "$output_pkg")"
productbuild \
  --distribution "$distribution" \
  --package-path "$work_dir" \
  "$output_pkg"

echo "Created installer: $output_pkg"
