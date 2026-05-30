import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import { ThemeProvider } from "@galacean/editor-ui";
// dockview docking workbench theme (the editor shell uses DockviewReact).
import "dockview/dist/styles/dockview.css";
import "./index.css";
import App from "./App";

// React-Scan is installed by the localhost-gated <script> tag in
// `index.html` — kept out of the bundle so production deploys don't
// ship the dev tool, and loaded synchronously *before* this module so
// it can patch React-DOM in time for the first render.

// `ThemeProvider` (galacean editor-ui) installs the stitches dark theme class
// on <html>, which the editor-ui control kit (inputs, selects, trees, color
// pickers, …) reads. It coexists with the Tailwind dark palette in index.css.
createRoot(document.getElementById("root")!).render(
  <StrictMode>
    <ThemeProvider>
      <App />
    </ThemeProvider>
  </StrictMode>,
);
