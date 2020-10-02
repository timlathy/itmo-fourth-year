plugins {
    java
    kotlin("jvm") version "1.4.10"
}

group = "itmo.se"
version = "1.0-SNAPSHOT"

repositories {
    mavenCentral()
}

dependencies {
    implementation(kotlin("stdlib"))
    testImplementation("org.junit.jupiter", "junit-jupiter", "5.7.0")
    testImplementation("org.mockito", "mockito-core", "3.5.13")
}

tasks.withType<Test> {
    useJUnitPlatform()
}