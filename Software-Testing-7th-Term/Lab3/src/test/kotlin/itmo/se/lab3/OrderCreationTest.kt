package itmo.se.lab3

import itmo.se.lab3.helpers.RunWithChrome
import itmo.se.lab3.helpers.WebDriverTemplateInvocationContextProvider
import itmo.se.lab3.helpers.value
import itmo.se.lab3.pages.OrderCreationPage
import org.junit.jupiter.api.Assertions.*
import org.junit.jupiter.api.TestTemplate
import org.junit.jupiter.api.extension.ExtendWith
import org.openqa.selenium.WebDriver

@ExtendWith(WebDriverTemplateInvocationContextProvider::class)
class OrderCreationTest {
    @TestTemplate
    @RunWithChrome
    fun `it allows the user to search through templates`(driver: WebDriver) {
        val orderPage = OrderCreationPage(driver)
        assertEquals("Базовый шаблон (копирайтинг)", orderPage.templateDropdownButton.text)
        assertEquals(0, orderPage.displayedTemplates.size)
        orderPage.openTemplates()
        assertNotEquals(0, orderPage.displayedTemplates.size)

        orderPage.templateSearchField.sendKeys("признание в чувствах")
        assertEquals(1, orderPage.displayedTemplates.size)
        assertEquals("Стихотворение, поэзия", orderPage.displayedTemplates[0].text)
    }

    @TestTemplate
    @RunWithChrome
    fun `it shows default order parameters when the page is opened`(driver: WebDriver) {
        val orderPage = OrderCreationPage(driver)
        assertEquals("Базовый шаблон (копирайтинг)", orderPage.templateDropdownButton.text)
        assertEquals("Написать статью (копирайтинг)", orderPage.orderNameField.value)
        assertEquals("2000", orderPage.orderMinSizeField.value)
        assertEquals("5000", orderPage.orderMaxSizeField.value)
    }

    @TestTemplate
    @RunWithChrome
    fun `it fills in order parameters based on the selected template`(driver: WebDriver) {
        val orderPage = OrderCreationPage(driver)
        assertEquals("Базовый шаблон (копирайтинг)", orderPage.templateDropdownButton.text)
        orderPage.openTemplates()
        orderPage.selectTemplate(orderPage.displayedTemplates.find { it.text == "Стихотворение, поэзия" }!!)
        assertEquals("Стихотворение, поэзия", orderPage.templateDropdownButton.text)

        assertEquals("Написать стихотворение", orderPage.orderNameField.value)
        assertEquals("500", orderPage.orderMinSizeField.value)
        assertEquals("2000", orderPage.orderMaxSizeField.value)
    }
}