//! Reflection-driven type registry + a JSON schema export for the editor.
//!
//! The control structs in [`super::control`] derive [`bevy_reflect::Reflect`],
//! so the editor's property grid and control toolbox can be generated from
//! the same definitions the engine uses — no second, hand-maintained schema
//! that silently drifts. [`registry_json`] walks the reflected field info
//! and emits a compact descriptor the studio reads as `control-registry.json`
//! (see the `--dump-registry` bin).

use bevy_reflect::{TypeInfo, TypeRegistry, Typed};
use serde_json::{json, Value};

use super::control::{Binding, ControlNode, Layout};
use super::document::{SceneDoc, StateInit};

/// The control kinds and which (kind-specific) fields apply to each — the
/// authoritative list the property grid filters by.
const KINDS: &[(&str, &[&str])] = &[
    ("group", &[]),
    ("sprite", &["handle", "frame"]),
    ("text", &["text", "size", "spacing", "anchor"]),
    ("quad", &["color"]),
    ("input_region", &[]),
    ("button", &["label", "size", "spacing", "color"]),
];

/// Fields common to every control (always shown in the property grid).
const COMMON: &[&str] = &["name", "state", "tint", "sx", "sy", "rot", "layout", "bindings"];

/// Property names a [`Binding`] may target.
const BINDABLE: &[&str] = &["visible", "x", "y", "frame", "text", "tint", "color"];

/// A `TypeRegistry` with every scene-document type registered. Useful for
/// dynamic (de)serialization and any future reflect-driven tooling.
pub fn type_registry() -> TypeRegistry {
    let mut r = TypeRegistry::new();
    r.register::<SceneDoc>();
    r.register::<StateInit>();
    r.register::<ControlNode>();
    r.register::<Layout>();
    r.register::<Binding>();
    r
}

/// Reflected `(field_name, type_path)` pairs for a struct type.
fn struct_fields(info: &TypeInfo) -> Vec<Value> {
    match info {
        TypeInfo::Struct(s) => s
            .iter()
            .map(|f| json!({ "name": f.name(), "type": short_type(f.type_path()) }))
            .collect(),
        _ => Vec::new(),
    }
}

/// Trim a fully-qualified type path to something the editor can switch on
/// (`alloc::string::String` -> `String`, `core::option::Option<f32>` ->
/// `Option<f32>`).
fn short_type(path: &str) -> String {
    // Keep generic args but shorten each path segment to its last component.
    fn last_seg(s: &str) -> &str {
        s.rsplit("::").next().unwrap_or(s)
    }
    if let Some(open) = path.find('<') {
        let (head, rest) = path.split_at(open);
        let inner = rest.trim_start_matches('<').trim_end_matches('>');
        format!("{}<{}>", last_seg(head), short_type(inner))
    } else {
        last_seg(path).to_string()
    }
}

/// Build the editor-facing control registry descriptor.
pub fn registry_json() -> Value {
    let kinds: Value = KINDS
        .iter()
        .map(|(k, fields)| (k.to_string(), json!(fields)))
        .collect::<serde_json::Map<_, _>>()
        .into();

    json!({
        "version": 1,
        "kinds": kinds,
        "common": COMMON,
        "bindable": BINDABLE,
        "types": {
            "ControlNode": struct_fields(ControlNode::type_info()),
            "Layout": struct_fields(Layout::type_info()),
            "Binding": struct_fields(Binding::type_info()),
            "SceneDoc": struct_fields(SceneDoc::type_info()),
        }
    })
}

/// Pretty-printed [`registry_json`] (what the `--dump-registry` bin writes).
pub fn registry_json_string() -> String {
    serde_json::to_string_pretty(&registry_json()).unwrap_or_else(|_| "{}".into())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn registry_lists_control_fields_and_kinds() {
        let v = registry_json();
        // Kinds present.
        assert!(v["kinds"]["sprite"].is_array());
        assert_eq!(v["kinds"]["sprite"][0], "handle");
        // Reflected ControlNode fields include the flat typed props.
        let fields = v["types"]["ControlNode"].as_array().unwrap();
        let names: Vec<&str> = fields.iter().map(|f| f["name"].as_str().unwrap()).collect();
        assert!(names.contains(&"handle"));
        assert!(names.contains(&"children"));
        assert!(names.contains(&"layout"));
    }

    #[test]
    fn short_type_shortens_paths_and_generics() {
        assert_eq!(short_type("alloc::string::String"), "String");
        assert_eq!(short_type("core::option::Option<f32>"), "Option<f32>");
        assert_eq!(short_type("f32"), "f32");
    }

    #[test]
    fn type_registry_registers_doc_types() {
        let r = type_registry();
        assert!(r.get(std::any::TypeId::of::<ControlNode>()).is_some());
        assert!(r.get(std::any::TypeId::of::<SceneDoc>()).is_some());
    }
}
