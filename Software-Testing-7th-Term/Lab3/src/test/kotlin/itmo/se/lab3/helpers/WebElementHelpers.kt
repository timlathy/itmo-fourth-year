package itmo.se.lab3.helpers

import org.openqa.selenium.WebElement

val WebElement.value: String get() = getAttribute("value")

/// Returns the substring matched by the first capturing group in the supplied regular expression
fun String.extract(re: Regex): String = re.find(this)!!.groups[1]!!.value