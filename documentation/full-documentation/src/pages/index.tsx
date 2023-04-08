import useDocusaurusContext from "@docusaurus/useDocusaurusContext";
import Layout from "@theme/Layout";
import React from "react";

import styles from "./index.module.css";

export default function Home(): JSX.Element {
    const {siteConfig} = useDocusaurusContext();
    return (
        <Layout
            description="Open Phybox">
            <main className={styles.main}>
                <img alt={"Logo"} width={150} height={150} src="img/open-phybox.svg" />
                <h1>Open Phybox</h1>
                <p>
                    Open Phybox is an open source re-implementation of the Phybox experiment box for schools. <br />
                    Compatible with the original Phybox measurement devices.
                </p>
            </main>
        </Layout>
    );
}