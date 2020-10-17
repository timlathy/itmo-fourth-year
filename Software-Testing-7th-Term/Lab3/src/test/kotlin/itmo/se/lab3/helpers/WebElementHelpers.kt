package itmo.se.lab3.helpers

import org.openqa.selenium.WebElement

val WebElement.value: String get() = getAttribute("value")